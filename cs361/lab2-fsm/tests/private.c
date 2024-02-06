#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../effects.h"
#include "../procmodel.h"
#include "../statemodel.h"

/* Check a state, liveness, and runtime */
#define CK_SLR(FSM, ST, LV, RT) \
  ck_assert_int_eq (FSM->state, ST); \
  ck_assert_int_eq (FSM->live, LV); \
  ck_assert_int_eq (FSM->runtime, RT);

/* Set the state, trigger an event, check results */
#define ST_TR_CK(FSM, FROM, EVT, TO, LV, RT) \
  FSM->state = FROM; \
  handle_event (FSM, EVT); \
  CK_SLR (FSM, TO, LV, RT);

/* Like above, but check only states */
#define ST_CK(FSM, FROM, EVT, TO) \
  FSM->state = FROM; \
  handle_event (FSM, EVT); \
  ck_assert_int_eq (FSM->state, TO);

/* MINIMUM: Checks state transitions only */
START_TEST (MIN_one_proc_valid_events_state_only)
{
  fsm_t *process = process_init ();
  assert (process != NULL);

  printf ("\n======================================\n");
  printf ("MINIMUM CREDIT: one_proc_valid_events_state_only\n");
  printf ("This test case sets the state, triggers an event, and checks that\n");
  printf ("the state is updated correctly.\n");

  // NEW events
  printf ("Testing valid events from NEW\n");
  ST_CK (process, NEW, ADMIT, RDY);
  ST_CK (process, NEW, KILL, TRM);

  // RDY events
  printf ("Testing valid events from RDY\n");
  ST_CK (process, RDY, DISPATCH, RUN);
  ST_CK (process, RDY, BLOCK, BLK);
  ST_CK (process, RDY, KILL, TRM);

  // RUN events
  printf ("Testing valid events from RUN\n");
  ST_CK (process, RUN, DISPATCH, RUN);
  ST_CK (process, RUN, SCHEDULE, RDY);
  ST_CK (process, RUN, BLOCK, BLK);
  ST_CK (process, RUN, EXIT, TRM);

  // BLK events
  printf ("Testing valid events from BLK\n");
  ST_CK (process, BLK, UNBLOCK, RDY);
  ST_CK (process, BLK, KILL, TRM);

  free (process);
}
END_TEST

START_TEST (MIN_one_proc_invalid_events_state_only)
{
  fsm_t *process = process_init ();
  assert (process != NULL);

  printf ("\n======================================\n");
  printf ("MINIMUM CREDIT: one_proc_invalid_events_state_only\n");
  printf ("This test case sets the state, triggers an invalid event, and\n");
  printf ("checks that the state does not change.\n");

  // NEW events
  printf ("Testing invalid events from NEW\n");
  ST_CK (process, NEW, DISPATCH, NEW);
  ST_CK (process, NEW, SCHEDULE, NEW);
  ST_CK (process, NEW, BLOCK, NEW);
  ST_CK (process, NEW, UNBLOCK, NEW);
  ST_CK (process, NEW, EXIT, NEW);

  // RDY events
  printf ("Testing invalid events from RDY\n");
  ST_CK (process, RDY, ADMIT, RDY);
  ST_CK (process, RDY, SCHEDULE, RDY);
  ST_CK (process, RDY, UNBLOCK, RDY);
  ST_CK (process, RDY, EXIT, RDY);

  // RUN events
  printf ("Testing invalid events from RUN\n");
  ST_CK (process, RUN, ADMIT, RUN);
  ST_CK (process, RUN, UNBLOCK, RUN);
  ST_CK (process, RUN, KILL, RUN);

  // BLK events
  printf ("Testing invalid events from BLK\n");
  ST_CK (process, BLK, ADMIT, BLK);
  ST_CK (process, BLK, DISPATCH, BLK);
  ST_CK (process, BLK, SCHEDULE, BLK);
  ST_CK (process, BLK, BLOCK, BLK);
  ST_CK (process, BLK, EXIT, BLK);
    
  // TRM events
  printf ("Testing invalid events from TRM\n");
  ST_CK (process, TRM, ADMIT, TRM);
  ST_CK (process, TRM, DISPATCH, TRM);
  ST_CK (process, TRM, SCHEDULE, TRM);
  ST_CK (process, TRM, BLOCK, TRM);
  ST_CK (process, TRM, UNBLOCK, TRM);
  ST_CK (process, TRM, EXIT, TRM);
  ST_CK (process, TRM, KILL, TRM);

  free (process);
}
END_TEST

void private_tests (Suite *s)
{
  TCase *tc_private = tcase_create ("Private");
  tcase_add_test (tc_private, MIN_one_proc_valid_events_state_only);
  tcase_add_test (tc_private, MIN_one_proc_invalid_events_state_only);
  suite_add_tcase (s, tc_private);
}

