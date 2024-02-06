#include <assert.h>
#include <check.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "../mutex.h"

double time_diff (struct timeval start, struct timeval end)
{
  double ending = end.tv_sec + (end.tv_usec * 0.000001);
  double starting = start.tv_sec + (start.tv_usec * 0.000001);
  return ending - starting;
}

START_TEST (MIN_mutex_run_two_threads)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN mutex run two threads\n");
  printf ("  Call runner in two threads and ensure result is 0\n");

  pthread_mutex_t lock;
  pthread_mutex_init (&lock, NULL);
  int64_t shared = run ();
  pthread_mutex_destroy (&lock);
  ck_assert_int_eq (shared, 0);
  printf ("\n");
}
END_TEST

START_TEST (MIN_mutex_run_two_threads_time)
{
  printf ("======================================\n");
  printf ("PRIVATE\n");
  printf ("  MIN mutex runner two threads time\n");
  printf ("  Call runner in two threads and measure time\n");
  printf ("  Multithreaded should take more than 3 times and\n");
  printf ("    less than 6 times the unithreaded time.\n");

  int64_t num = 0;
  int64_t *shared = &num;

  struct timeval start, end;
  gettimeofday (&start, NULL);
  for (int j = 0; j < 1000000; j++)
    {
      for (int i = 0; i < 100; i++)
        {
          *shared += 1;
          *shared -= 1;
        }
    }
  gettimeofday (&end, NULL);
  double unidiff = time_diff (start, end);

  pthread_mutex_t lock;
  pthread_mutex_init (&lock, NULL);
  gettimeofday (&start, NULL);
  num = run ();
  gettimeofday (&end, NULL);
  double multidiff = time_diff (start, end);

  printf ("\n  Unithreaded time: %f\n  Multithreaded time: %f\n\n",
          unidiff, multidiff);

  ck_assert_int_eq (num, 0);
  ck_assert (unidiff * 6 > multidiff);
  ck_assert (unidiff * 2.5 < multidiff);
  printf ("\n");

}
END_TEST

/*
   1 - Create a queue of size 1 and put things in one at a time, pausing
       for 50 ms in between.
   2 - Create a queue of size 8, fill it, then start the worker.
   3 - Create a queue of size 15, start the listener, then pull one item
       at a time.
   4 - Just use fifo_queue for size 10, 20, and 30.
*/

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_set_timeout (tc_public, 30.0);
  tcase_add_test (tc_public, MIN_mutex_run_two_threads);
  tcase_add_test (tc_public, MIN_mutex_run_two_threads_time);
  suite_add_tcase (s, tc_public);
}
