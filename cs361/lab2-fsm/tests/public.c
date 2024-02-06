#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../effects.h"
#include "../procmodel.h"
#include "../statemodel.h"

/* nothing but admitting a new process */
START_TEST (MIN_admit_and_stop)
{
  fsm_t *fsm = process_init ();
  assert (fsm != NULL);

  handle_event (fsm, ADMIT);
  ck_assert_int_eq (fsm->state, RDY);

  free (fsm);
}
END_TEST

/* a single process from NEW to TRM */
START_TEST (MIN_one_proc_linear_state_only)
{
  fsm_t *fsm = process_init ();
  assert (fsm != NULL);

  handle_event (fsm, ADMIT);
  ck_assert_int_eq (fsm->state, RDY);

  handle_event (fsm, DISPATCH);
  ck_assert_int_eq (fsm->state, RUN);

  handle_event (fsm, EXIT);
  ck_assert_int_eq (fsm->state, TRM);

  free (fsm);
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, MIN_admit_and_stop);
  tcase_add_test (tc_public, MIN_one_proc_linear_state_only);
  suite_add_tcase (s, tc_public);
}

