#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

#include "effects.h"

/* Set the FSM runtime counter to 0 */
void
reset_runtime (fsm_t *fsm)
{
  assert (fsm != NULL);
  fsm->runtime = 0;
}

/* Indicate the process is live */
void
set_live (fsm_t *fsm)
{
  assert (fsm != NULL);
  fsm->live = true;
  // printf ("  [Process is live]\n");
}

/* Indicate the process has been killed */
void
kill_proc (fsm_t *fsm)
{
  assert (fsm != NULL);
  fsm->live = false;
}

/* Increment the run time counter by 1 */
void
incr_runtime (fsm_t *fsm)
{
  assert (fsm != NULL);
  fsm->runtime++;
}

/* Print the message that the process is blocked */
void
say_blocked (fsm_t *fsm)
{
  printf ("  [Process is blocked]\n");
}

/* Print the message that the process has been unblocked */
void
say_unblocked (fsm_t *fsm)
{
  printf ("  [Process is unblocked]\n");
}

/* Print the process statistics (runtime and live status) */
void
print_stats (fsm_t *fsm)
{
  printf ("\n  Process stats:\n");
  printf ("    runtime: %" PRIu32 "\n", fsm->runtime);
  printf ("    normal exit: %s\n", (fsm->live ? "true" : "false"));
}
