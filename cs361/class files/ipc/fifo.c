#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "headers.h"

#define BUFFER 100

void
send_msg (char * fifo)
{
  printf ("What would you like to say?\n");
  char buffer[BUFFER];
  memset (buffer, 0, sizeof (buffer));
  read (STDIN_FILENO, buffer, BUFFER - 1);
  
  char *newline = strchr (buffer, '\n');
  if (newline != NULL)
    *newline = '\0';

  fprintf (stderr, "[Opening FIFO]\n");
  int fd = open (fifo, O_WRONLY);
  // Perhaps it doesn't exist so try to make it
  if (fd < 0)
    {
      fprintf (stderr, "[FIFO doesn't exist. Creating it and opening]\n");
      assert (mkfifo (fifo, S_IRUSR | S_IWUSR) == 0);
      fd = open (fifo, O_RDWR);
    }
  assert (fd >= 0);
  fprintf (stderr, "[Sending message '%s']\n", buffer);
  ssize_t bytes = write (fd, buffer, strlen (buffer) + 1);
  fprintf (stderr, "[Sent %zd bytes]\n", bytes);
  close (fd);
}

void
retrieve_msg (char * fifo)
{
  fprintf (stderr, "[Opening FIFO]\n");
  int fd = open (fifo, O_RDONLY);
  assert (fd >= 0);

  fprintf (stderr, "Checking for messages...\n");
  char buffer[BUFFER];
  memset (buffer, 0, sizeof (buffer));
  ssize_t bytes = read (fd, buffer, BUFFER - 1);
  close (fd);
  fprintf (stderr, "[Received %zd bytes]\n", bytes);
  printf ("%s\n", buffer);
}
