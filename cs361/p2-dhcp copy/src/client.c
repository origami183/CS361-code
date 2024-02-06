#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>

#include "dhcp.h"
#include "format.h"
#include "port_utils.h"

static int get_args(int, char **, uint32_t *, uint8_t *, uint8_t *, unsigned char *, unsigned char *, char *);

void c_print(msg_t *packet)
{
  dump_msg(stdout, packet, sizeof(*packet));
  printf("\n++++++++++++++++\n");
  printf("SERVER RECEIVED:\n");
  dump_msg(stdout, packet, sizeof(*packet));
  printf("\n++++++++++++++++\n");
}

int main(int argc, char **argv)
{
  msg_t *packet = (msg_t *)malloc(sizeof(msg_t));
  memset(packet, 0, sizeof(msg_t));

  // Defualts
  uint8_t op = 1;
  uint32_t xid = htonl(42);
  uint8_t htype = ETH;
  uint8_t hlen = ETH_LEN;

  unsigned char chaddr[16];
  // memset(packet->chaddr, 0x010203040506, sizeof(packet->chaddr));
  for (int i = 0; i < ETH_LEN; i++)
  {
    chaddr[i] = i + 1;
  }

  unsigned char options[MAX_DHCP_LENGTH];
  // memset(packet->options, MAGIC_COOKIE, sizeof(uint32_t));
  options[0] = 0x63;
  options[1] = 0x82;
  options[2] = 0x53;
  options[3] = 0x63;

  //
  memset(options + 6, DHCPDISCOVER, sizeof(uint8_t));

  // Request
  options[9] = 0x7f;
  options[10] = 0x00;
  options[11] = 0x00;
  options[12] = 0x02;

  // Identifier
  options[15] = 0x7f;
  options[16] = 0x00;
  options[17] = 0x00;
  options[18] = 0x01;

  char sname[16];
  int flag = get_args(argc, argv, &xid, &htype, &hlen, chaddr, options, sname);

  fill_msg(packet, op, htype, hlen, xid, chaddr, options);

  if (flag && (xid > 0))
  {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;

    char *custom_port_str = get_port();
    unsigned short custom_port = (unsigned short)atoi(custom_port_str);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(custom_port);
    server_addr.sin_addr.s_addr = htonl(0x7f000001);

    // Time out for socket
    struct timeval timeout;
    timeout.tv_sec = 1.0;
    timeout.tv_usec = 250000;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));

    int loops = 10;
    // while (condition)
    while (loops > 0)
    {
      if (packet->options[6] == DHCPDISCOVER)
      {
        dump_msg(stdout, packet, sizeof(*packet));
        printf("++++++++++++++++\n");
        sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Recieved Packet
        msg_t *recieved_packet = (msg_t *)malloc(sizeof(msg_t));
        memset(recieved_packet, 0, sizeof(msg_t));

        socklen_t client_addr_len = sizeof(client_addr);
        bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ssize_t bytes_received = recvfrom(sockfd, recieved_packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        // Printing
        if (bytes_received > 0 && recieved_packet->options[6] == DHCPOFFER)
        {
          printf("CLIENT RECEIVED:\n");
          dump_msg(stdout, recieved_packet, sizeof(*recieved_packet));
          printf("++++++++++++++++\n");
        }
        else if (bytes_received > 0 && recieved_packet->options[6] == DHCPNAK)
        {
          printf("CLIENT RECEIVED:\n");
          dump_msg(stdout, recieved_packet, sizeof(*recieved_packet));
          printf("++++++++++++++++\n");
          free(recieved_packet);
          break;
        }

        memcpy(packet, recieved_packet, sizeof(msg_t));
        packet->op = BOOTREQUEST;

        memcpy(&packet->options[9], &packet->yiaddr.s_addr, 4);

        packet->yiaddr.s_addr = 0x00000000;

        packet->options[6] = DHCPREQUEST;

        free(recieved_packet);
      }
      else if (packet->options[6] == DHCPREQUEST)
      {
        dump_msg(stdout, packet, sizeof(*packet));
        printf("++++++++++++++++\n");
        sendto(sockfd, packet, sizeof(*packet), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

        // Recieved Packet
        msg_t *recieved_packet = (msg_t *)malloc(sizeof(msg_t));
        memset(recieved_packet, 0, sizeof(msg_t));

        socklen_t client_addr_len = sizeof(client_addr);
        bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
        ssize_t bytes_received = recvfrom(sockfd, recieved_packet, sizeof(msg_t), 0, (struct sockaddr *)&client_addr, &client_addr_len);

        // Printing
        if (bytes_received > 0)
        {
          printf("CLIENT RECEIVED:\n");
          dump_msg(stdout, recieved_packet, sizeof(*recieved_packet));
          printf("++++++++++++++++\n");
        }

        if (recieved_packet->options[6] == DHCPACK)
        {
          loops = 1;
        }

        free(recieved_packet);
      }

      // Clean up
      loops--;
    }

    close(sockfd);
  }
  else if (flag && (xid == 0))
  {
    dump_msg(stdout, packet, sizeof(*packet));
    printf("\n++++++++++++++++\n");
    printf("SERVER RECEIVED:\n");
    dump_msg(stdout, packet, sizeof(*packet));
    printf("++++++++++++++++\n");
  }
  else
  {
    dump_msg(stdout, packet, sizeof(*packet));
  }

  return EXIT_SUCCESS;
}

static int
get_args(int argc, char **argv, uint32_t *xid, uint8_t *htype, uint8_t *hlen, unsigned char *chaddr, unsigned char *options, char *sname)
{
  // printing type: 0 normal, 1 server;
  int flag = 0;

  int ch = 0;
  while ((ch = getopt(argc, argv, "x:t:c:m:s:r:p")) != -1)
  {
    switch (ch)
    {
    // • -x N : use N as the XID field (32-bit unsigned integer) [default 42]
    case 'x':
      *xid = htonl(atoi(optarg));
      break;
    // • -t N : use N as the hardware type (must be one named in src/dhcp.h) [default ETH]
    case 't':
      *htype = (uint8_t)atoi(optarg);
      switch ((uint8_t)atoi(optarg))
      {
      case ETH:
        *hlen = ETH_LEN;
        break;
      case IEEE802:
        *hlen = IEEE802_LEN;
        break;
      case ARCNET:
        *hlen = ARCNET_LEN;
        break;
      case FRAME_RELAY:
        *hlen = FRAME_LEN;
        break;
      case FIBRE:
        *hlen = FIBRE_LEN;
        break;
      case ATM:
        *hlen = ATM_LEN;
        break;
      }
      break;
    // • -c N : use N as the hardware address ( chaddr ) [default 0102...]
    case 'c':
    {
      int offset = 0;
      if (*hlen != ATM_LEN)
      {
        for (int i = 0; i < *hlen; i++)
        {
          sscanf(optarg + offset, "%2hhx", chaddr + i);
          offset += 2;
        }
      }
      else
      {
        for (int i = 0; i < *hlen; i++)
        {
          if (i < 16)
          {
            sscanf(optarg + offset, "%2hhx", chaddr + i);
          }
          else
          {
            sscanf(optarg + offset, "%2hhx", (unsigned char *)sname + i - 16);
          }
          offset += 2;
        }
      }
    }
    break;
    // • -m N : create DHCP message type M [default DHCPDISCOVER]
    case 'm':
      memset(options + 6, atoi(optarg), sizeof(uint8_t));
      break;
    // • -s N.N.N.N : specify the server IP DHCP option [default 127.0.0.1]
    case 's':
    {
      uint8_t type = 0;
      memcpy(&type, options + 6, sizeof(uint8_t));

      uint8_t offset = 9;
      if (type == DHCPREQUEST || type == DHCPDECLINE)
        offset = 15;

      char *address = optarg;
      char *token = strtok(address, ".");
      for (int i = 0; i < 4; i++)
      {
        options[offset + i] = atoi(token);
        token = strtok(NULL, ".");
      }
    }
    break;
    // • -r N.N.N.N : specify the requested IP DHCP option [default [127.0.0.2]
    case 'r':
    {
      char *address = optarg;
      char *token = strtok(address, ".");
      for (int i = 0; i < 4; i++)
      {
        options[9 + i] = atoi(token);
        token = strtok(NULL, ".");
      }
    }
    break;
    // • -p : initiate the protocol (send UDP packet)
    case 'p':
    {
      flag = 1;
    }
    break;
    }
  }
  return flag;
}
