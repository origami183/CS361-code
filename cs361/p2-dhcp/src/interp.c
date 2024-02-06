#include <stdlib.h>

#include "dhcp.h"
#include "format.h"
#include <string.h>
#include <sys/stat.h>

int
main (int argc, char **argv)
{
  // TODO: Open the file specified on the command line
  // Note: The file contains a binary network packet
  FILE *file = fopen (argv[1], "rb");

  // TODO: Determine the size of the packet in bytes
  // Note: You have used the fstat() function before
  struct stat file_stat; 
  fstat (fileno (file), &file_stat);
  size_t packet_size = file_stat.st_size;

  // TODO: Define a uint8_t * variable
  msg_t *packet;

  // TODO: Allocate enough space to hold the packet (store in your varaible)
  packet = (msg_t *)malloc (packet_size);

  // TODO: Read the packet data from the file into your variable
  fread (packet, 1, packet_size, file); 


  // TODO: Call dump_msg() from format.c to print the packet data to stdout
  dump_msg(stdout, packet, packet_size);

  // TODO: Don't forget to free the dynamically-allocated space
  fclose (file);
  free (packet);

  return EXIT_SUCCESS;
}
