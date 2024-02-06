#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

static bool get_args(int, char **);

static bool debug = false;
static int binds = 0;
static uint32_t newYI = 0xa000201;

void prepare_packet(msg_t *packet)
{
  newYI += binds;
  packet->yiaddr.s_addr = htonl(newYI);
  packet->options[6] = DHCPOFFER;
  packet->op = BOOTREPLY;

  packet->options[21] = 1;
  memcpy(&packet->options[22], &packet->yiaddr.s_addr, 4);

  packet->options[9] = 0x00;
  packet->options[10] = 0x27;
  packet->options[11] = 0x8d;
  packet->options[12] = 0x00;

  packet->options[15] = 0x0a;
  packet->options[16] = 0x00;
  packet->options[17] = 0x02;
  packet->options[18] = 0x00;
}

void prepare_packet2(msg_t *packet)
{
  packet->yiaddr.s_addr = htonl(newYI);
  packet->options[6] = DHCPACK;
  packet->op = BOOTREPLY;

  packet->options[21] = 0;

  // Request
  packet->options[9] = 0x00;
  packet->options[10] = 0x27;
  packet->options[11] = 0x8d;
  packet->options[12] = 0x00;

  packet->options[15] = 0x0a;
  packet->options[16] = 0x00;
  packet->options[17] = 0x02;
  packet->options[18] = 0x00;
}

int main(int argc, char **argv)
{
  // Server recieved

  get_args(argc, argv);

  struct sockaddr_in server_addr, client_addr;

  char *custom_port_str = get_port();
  short port = (short)(atoi(custom_port_str));

  int sockfd;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(0x7f000001); // 127.0.0.1

  socklen_t client_addr_len = sizeof(client_addr);

  // Time out for socket
  struct timeval timeout;
  timeout.tv_sec = 2.0;
  timeout.tv_usec = 500000;
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

  // Server Binding
  bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

  // Packet
  msg_t *packet = (msg_t *)malloc(sizeof(msg_t));
  memset(packet, 0, sizeof(msg_t));

  // Server Condition
  int run_server = 1;
  bool isANak = false;
  while (run_server)
  {
    /* Recieve 1 */
    ssize_t bytes_recieved = recvfrom(sockfd, packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, &client_addr_len);

    if (bytes_recieved == -1)
    {
      break;
    }
    else
    {
      printf("++++++++++++++++\nSERVER RECEIVED:\n");
      dump_msg(stdout, packet, sizeof(msg_t));
    }

    /* Prepare */
    prepare_packet(packet);

    /* NAK Check */
    if (binds == 10)
    {
      packet->options[6] = DHCPNAK;
      packet->yiaddr.s_addr = 0;

      packet->options[9] = 0x0a;
      packet->options[10] = 0x00;
      packet->options[11] = 0x02;
      packet->options[12] = 0x00;

      memset(&packet->options[13], 0, 12);
      isANak = true;
    }
    /* Send 1 */
    sendto(sockfd, packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (bytes_recieved == -1)
    {
      break;
    }
    else
    {
      printf("++++++++++++++++\nSERVER SENDING:\n");
      dump_msg(stdout, packet, sizeof(msg_t));
    }
    /* NAK ESCAPE */
    if (isANak == true)
    {
      // printf("STAHP! IS A NAK\n");
      break;
    }

    /* Recieve 2 */
    bytes_recieved = recvfrom(sockfd, packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    if (bytes_recieved == -1)
    {
      break;
    }
    else
    {
      printf("++++++++++++++++\nSERVER RECEIVED:\n");
      dump_msg(stdout, packet, sizeof(msg_t));
    }
    /* Prepare 2 */
    prepare_packet2(packet);
    /* Send 2 */
    bytes_recieved = sendto(sockfd, packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (bytes_recieved == -1)
    {
      break;
    }
    else
    {
      printf("++++++++++++++++\nSERVER SENDING:\n");
      dump_msg(stdout, packet, sizeof(msg_t));
    }

    newYI = 0xa000201;
    binds++; // New connection success! :D
    // binds += 1;
    if (binds > 10)
    { // Maximimum connections allowed; others probably died. Reset to 0.
      binds = 0;
    }

    if (debug)
    {
      printf("Received a message from client: %ld\n", bytes_recieved);
      dump_msg(stdout, packet, sizeof(msg_t));
    }
  }

  // Clean up
  free(packet);

  close(sockfd);
  if (debug)
    fprintf(stderr, "Shutting down\n");

  return EXIT_SUCCESS;
}

static bool
get_args(int argc, char **argv)
{
  int ch = 0;
  while ((ch = getopt(argc, argv, "dh")) != -1)
  {
    switch (ch)
    {
    case 'd':
      debug = true;
      break;
    default:
      return false;
    }
  }
  return true;
}
