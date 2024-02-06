#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "headers.h"
#include "parse.h"

int
main (int argc, char **argv)
{
  int sample = -1;

  if (parse_command_line (argc, argv, &sample) != 0)
    return EXIT_FAILURE;

  switch (sample)
    {
    case 1:
    case 2:
      run_fork (sample);
      break;
    case 3:
      run_exec (false);
      break;
    case 4:
      run_exec (true);
      break;
    default:
      printf ("Invalid case number %d\n", sample);
    }

  return 13;
}
