/*
 * CS 361: Template lab driver
 *
 * Name: Grayson Diehl
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "procmodel.h"
#include "statemodel.h"

void
usage (void)
{
  printf ("fsm, a state model simulator\n\n");
  printf ("usage: fsm [evt1 evt2 ...]\n");
  printf (
      "evt1 evt2 ... is a sequence of event combinations. Each event is\n");
  printf ("represented by an int value:\n\n");
  printf ("  ADMIT = 0\n");
  printf ("  DISPATCH = 1\n");
  printf ("  SCHEDULE = 2\n");
  printf ("  BLOCK = 3\n");
  printf ("  UNBLOCK = 4\n");
  printf ("  EXIT = 5\n");
  printf ("  KILL = 6\n");
  printf ("  NIL = 7\n");
}

int
main (int argc, char **argv)
{
  // With no arguments, show usage and exit
  if (argc < 2)
    {
      usage ();
      return EXIT_FAILURE;
    }

  // Declare and initialize a FSM
  fsm_t *process = process_init ();
  if (process == NULL)
    return EXIT_FAILURE;

  // TODO: Replace this with a loop for all events.
  for (int i = 1; i < argc; i++)
    {
      char *event = argv[i];
      int eventValue = atoi (event);
      switch (eventValue)
        {
        case 0:
          handle_event (process, ADMIT);
          break;
        case 1:
          handle_event (process, DISPATCH);
          break;
        case 2:
          handle_event (process, SCHEDULE);
          break;
        case 3:
          handle_event (process, BLOCK);
          break;
        case 4:
          handle_event (process, UNBLOCK);
          break;
        case 5:
          handle_event (process, EXIT);
          break;
        case 6:
          handle_event (process, KILL);
          break;
        case 7:
          handle_event (process, NIL);
          break;
        default:
          usage ();
          free (process);
          return EXIT_FAILURE;
        }
    }

  // Under normal runs, we should always end in TRM state
  if (process->state != TRM)
    {
      free (process);
      return EXIT_FAILURE;
    }

  free (process);
  return EXIT_SUCCESS;
}
