#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "dhcp.h"
#include "format.h"

void dump_msg(FILE *output, msg_t *msg, size_t size)
{
  fprintf(output, "------------------------------------------------------\n");
  fprintf(output, "BOOTP Options\n");
  fprintf(output, "------------------------------------------------------\n");

  // TODO: Print out the BOOTP fields as specified in the assignment

  // Op Code:
  char *op = NULL;
  switch (msg->op)
  {
  case BOOTREQUEST:
    op = "BOOTREQUEST";
    break;
  case BOOTREPLY:
    op = "BOOTREPLY";
    break;
  }
  fprintf(output, "Op Code (op) = %d [%s]\n", msg->op, op);

  // Hardware Type:
  char *htype;
  switch (msg->htype)
  {
  case ETH: // 1
    htype = "Ethernet (10Mb)";
    break;
  case IEEE802: // 6
    htype = "IEEE 802 Networks";
    break;
  case ARCNET: // 7
    htype = "ARCNET";
    break;
  case FRAME_RELAY: // 15
    htype = "Frame Relay";
    break;
  case FIBRE: // 18
    htype = "Fibre Channel";
    break;
  case ATM: // 19
    htype = "Asynchronous Transmission Mode (ATM)";
    break;
  }
  fprintf(output, "Hardware Type (htype) = %d [%s]\n", msg->htype, htype);

  // Hardware Address Length:
  fprintf(output, "Hardware Address Length (hlen) = %d\n", msg->hlen);

  // Hops:
  fprintf(output, "Hops (hops) = %d\n", msg->hops);

  // Transaction ID:
  fprintf(output, "Transaction ID (xid) = %d (0x%x)\n", ntohl(msg->xid), ntohl(msg->xid));

  // Date and time:
  int seconds = ntohs(msg->secs);
  int minutes = 0;
  int hours = 0;
  int days = 0;
  while (seconds >= 60)
  {
    seconds = seconds - 60;
    minutes++;

    if (minutes == 60)
    {
      hours++;
      minutes = 0;
    }

    if (hours == 24)
    {
      days++;
      hours = 0;
    }
  }
  fprintf(output, "Seconds (secs) = %d Days,%2d:%02d:%02d\n", days, hours, minutes, seconds);

  // Flags:
  fprintf(output, "Flags (flags) = %d\n", msg->flags);

  // Client IP Address:
  fprintf(output, "Client IP Address (ciaddr) = %s\n", inet_ntoa(msg->ciaddr));

  // Your IP Address:
  fprintf(output, "Your IP Address (yiaddr) = %s\n", inet_ntoa(msg->yiaddr));

  // Server IP Address:
  fprintf(output, "Server IP Address (siaddr) = %s\n", inet_ntoa(msg->siaddr));

  // Relay IP Address:
  fprintf(output, "Relay IP Address (giaddr) = %s\n", inet_ntoa(msg->giaddr));

  // Client Ethernet Address
  fprintf(output, "Client Ethernet Address (chaddr) = ");
  for (int i = 0; i < msg->hlen; i++)
  {
    fprintf(output, "%02x", msg->chaddr[i]);
  }
  fprintf(output, "\n");

  if (size <= 236)
  {
    return;
  }

  fprintf(output, "------------------------------------------------------\n");
  fprintf(output, "DHCP Options\n");
  fprintf(output, "------------------------------------------------------\n");

  // TODO: Print out the DHCP fields as specified in the assignment

  uint32_t cookie = 0;
  memcpy(&cookie, msg->options, sizeof(uint32_t));

  if (ntohl(cookie) == MAGIC_COOKIE) // DHCP Magic Cookie
  {
    fprintf(output, "Magic Cookie = [OK]\n");
  }

  uint8_t mess_type = 0;
  memcpy(&mess_type, msg->options + 6, sizeof(uint8_t));
  fprintf(output, "Message Type = ");

  switch (mess_type) // DHCP Message Type
  {
  case DHCPDISCOVER: // 1
    fprintf(output, "DHCP Discover\n");
    break;
  case DHCPOFFER: // 2
    fprintf(output, "DHCP Offer\n");
    break;
  case DHCPREQUEST: // 3
    fprintf(output, "DHCP Request\n");
    break;
  case DHCPDECLINE: // 4
    fprintf(output, "DHCP Decline\n");
    break;
  case DHCPACK: // 5
    fprintf(output, "DHCP ACK\n");
    break;
  case DHCPNAK: // 6
    fprintf(output, "DHCP NAK\n");
    break;
  case DHCPRELEASE: // 7
    fprintf(output, "DHCP Release\n");
    break;
  }

  // TODO: adjust these so that the all three can be printed in one go
  // current idea: hidden conditional hidden in options array placed in server side DHCP offer packet
  // if hidden conditional is true, then print request & regular offer and ACK data
  // WE WILL NEED TO DETERMINE WHERE THE REQUEST IP ADDRESS IS STORED IN THE OPTIONS ARRAY
  if (mess_type == 2 || mess_type == 5) // IP Address Lease Time ie - DHCP Offer or ACK
  {
    uint32_t raw_time = 0;
    memcpy(&raw_time, msg->options + 9, sizeof(uint32_t));
    uint32_t new_Seconds = ntohl(raw_time);
    uint32_t new_Minutes = 0;
    uint32_t new_Hours = 0;
    uint32_t new_Days = 0;
    while (new_Seconds >= 60)
    {
      new_Seconds = new_Seconds - 60;
      new_Minutes++;

      if (new_Minutes == 60)
      {
        new_Hours++;
        new_Minutes = 0;
      }

      if (new_Hours == 24)
      {
        new_Days++;
        new_Hours = 0;
      }
    }

    if (msg->options[21] == 1)
      {
        struct in_addr request;
        memcpy (&request, msg->options + 22, sizeof (uint32_t));
        fprintf (output, "Request = %s\n", inet_ntoa (request));
      }    

    fprintf(output, "IP Address Lease Time = %d Days, %d:%02d:%02d\n",
            new_Days, new_Hours, new_Minutes, new_Seconds);

    struct in_addr ident;
    memcpy(&ident, msg->options + 15, sizeof(uint32_t));
    fprintf(output, "Server Identifier = %s\n", inet_ntoa(ident));
  }

  if (mess_type == 3 || mess_type == 4) // Request ie - DHCP Request or Decline
  {
    struct in_addr request;
    memcpy(&request, msg->options + 9, sizeof(uint32_t));
    fprintf(output, "Request = %s\n", inet_ntoa(request));

    struct in_addr ident;
    memcpy(&ident, msg->options + 15, sizeof(uint32_t));
    fprintf(output, "Server Identifier = %s\n", inet_ntoa(ident));
  }

  if (mess_type == 6 || mess_type == 7) // Server Identifier only ie-DHCP NAK or Release
  {
    struct in_addr ident;
    memcpy(&ident, msg->options + 9, sizeof(uint32_t));
    fprintf(output, "Server Identifier = %s\n", inet_ntoa(ident));
  }
}
