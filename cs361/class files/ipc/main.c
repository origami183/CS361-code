#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "headers.h"
#include "parse.h"

int
main (int argc, char **argv)
{
  int sample = -1;

  char * fifo = NULL;
  if (parse_command_line (argc, argv, &sample, &fifo) != 0)
    return EXIT_FAILURE;

  switch (sample)
    {
    case 1:
      run_pipe ();
      break;
    case 2:
      run_dup (2, 1);
      break;
    case 3:
      run_dup (1, 2);
      break;
    case 4:
      send_msg (fifo);
      break;
    case 5:
      retrieve_msg (fifo);
      break;
    case 6:
      run_tictac ();
      break;
    default:
      printf ("Invalid case number %d\n", sample);
    }

  return 13;
}
