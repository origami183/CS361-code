#include <ctype.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"

/*
 * This solution for parse_command_line is derived from the GNU getopt()
 * example available at:
 *
 * http://www.gnu.org/software/libc/manual/html_node/Example-of-Getopt.html#Example-of-Getopt
 */

/**
 * Prints the standard command-line usage message
 */
void
usage (void)
{
  printf ("Usage: ipc [options]\n");
  printf (" Options are:\n");
  printf ("  -h        Display usage\n");
  printf ("  -p        Run the pipe demo\n");
  printf ("  -d        Run the file redirection demo\n");
  printf ("  -s FILE   Send a message into the fifo named FILE\n");
  printf ("  -r FILE   Receive messages from the FILE fifo\n");
  printf ("  -t        Play tic-tac-toe\n");
}

#define BAILOUT(f) { printf ("Second flag %s not allowed\n", (f)); return 2; }

/**
 * Parses the command line; returns 1 for help and 2 for error
 */
int
parse_command_line (int argc, char **argv, int *sample, char **fifo)
{
  int option;

  /* Check for NULL parameters or insufficient number of parameters */
  if (argv == NULL || sample == NULL)
    return 2;

  *sample = -1;

  while ((option = getopt (argc, argv, "pd:s:r:th")) != -1)
    switch (option)
      {
      /* Bail out if -h ("help") flag was passed */
      case 'h':
        usage ();
        return 1;

      case 'p':
        if (*sample != -1) BAILOUT("-p");
        *sample = 1;
        break;

      case 'd':
        if (*sample != -1) BAILOUT("-d");
        if (strcmp (optarg, "2>&1"))
          *sample = 2;
        else if (strcmp (optarg, "1>&2"))
          *sample = 3;
        else
          {
            printf ("Dup version must specify 2>1 or 1>2\n");
            return 2;
          }
        break;

      case 's':
        *sample = 4;
        *fifo = optarg;
        break;

      case 'r':
        *sample = 5;
        *fifo = optarg;
        break;

      case 't':
        *sample = 6;
        break;

      case '?':
        /* Illegal flag or unknown character */
        if (isprint (optopt))
          printf ("Illegal flag `-%c'\n", optopt);
        else
          printf ("Unknown option character `\\x%x\n", optopt);
        return 2;

      default:
        /* should never happen */
        abort ();
      }

  if (*sample == -1)
    {
      printf ("Must pass exactly one flag\n");
      return 2;
    }

  return 0;
}
