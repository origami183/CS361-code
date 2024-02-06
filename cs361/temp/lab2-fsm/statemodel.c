#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "effects.h"
#include "procmodel.h"
#include "statemodel.h"

/* Generic front-end for handling events. Should do nothing more
   than calling the appropriate *_event function based on the type. */
void
handle_event (fsm_t *fsm, event_t event)
{
  assert (fsm);
  // Event and state names for printing out
  static const char *event_names[]
      = { "ADMIT",   "DISPATCH", "SCHEDULE", "BLOCK",
          "UNBLOCK", "EXIT",     "KILL",     "NIL" };
  static const char *state_names[]
      = { "NEW", "RDY", "RUN", "BLK", "TRM", "NST" };

  action_t effect = NULL;
  action_t entry = NULL;
  state_t nextState = fsm->transition (fsm, event, &effect, &entry);

  if (nextState == -1)
    {
      return;
    }
  else
    {
      printf ("%s.%s -> %s\n", state_names[fsm->state], event_names[event],
              state_names[nextState]);
    }

  if (nextState == NST)
    {
      return;
    }
  else
    {
      fsm->state = nextState;
    }

  if (effect != NULL)
    {
      effect (fsm);
    }

  // if (entry != NULL)
  //   {
  //     entry (fsm);
  //   }

  return;
}
