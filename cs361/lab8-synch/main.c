/*
 * CS 361: Template project driver
 *
 * Name:
 */

#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "pingpong.h"

int cmdline (int, char **, bool *, bool *);

void
usage (void)
{
  printf ("Usage: synch option [option...]\n");
  printf (" At least one argument must be passed\n");
  printf (" Options are:\n");
  printf ("  -m      Execute the runner() threads to demonstrate locks\n");
  printf ("  -p      Run pingong() to demonstrate semaphore-based timing\n");
}

int
main (int argc, char **argv)
{
  bool run_mutex = false;
  bool run_pingpong = false;
  if (cmdline (argc, argv, &run_mutex, &run_pingpong) < 0)
    {
      usage ();
      return EXIT_FAILURE;
    }

  // Use mutexes for mutual exclusion
  if (run_mutex)
    {
      printf ("Running mutual exclusion test\n");
      int64_t shared = run ();
      printf ("Value of shared: %" PRId64 "\n", shared);
    }

  // Use semaphores to demonstrate signaling
  if (run_pingpong)
    {
      printf ("Running a controlled timing exercise\n");
      int result = ping ();
      printf ("Value of result should be 0: %d\n", result);
    }

  pthread_exit (NULL);
}

/*****************************************************************************
 ****************** DO NOT MODIFY FUNCTIONS IN THIS SECTION ******************
 *****************************************************************************/

int
cmdline (int argc, char **argv, bool *mutex, bool *pingpong)
{
  int option;

  while ((option = getopt (argc, argv, "mph")) != -1)
    {
      switch (option)
        {
        case 'm':
          *mutex = true;
          break;
        case 'p':
          *pingpong = true;
          break;
        case 'h':
          return -1;
          break;
        default:
          return -1;
        }
    }

  if (!mutex && !pingpong)
    {
      printf ("You must pass at least one argument\n");
      usage ();
      return -1;
    }
  return 0;
}
