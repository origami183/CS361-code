#include <assert.h>
#include <check.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../discrete.h"
#include "../pthread.h"

uint64_t large_primes[] = {
  22181233, 30722903, 39410039, 48210479, 23878483, 32452597, 41161361,
  49978519, 25582069, 34185953, 42918971, 51754777, 27290279, 35925737,
  44679787, 53532901, 29005421, 37666553, 46439677, 55316903
};

uint64_t large_gens[] = {
  11090619, 15361451, 19705019, 24105239, 11939242, 16226298, 20580682,
  24989259, 12791034, 17092978, 21459486, 25877391, 13645139, 17962870,
  22339897, 26766452, 14502710, 18833278, 23219838, 27658451
};

uint64_t mod_powers[] = {
  10722318, 10718795, 16846904, 28514080, 13198105, 21563282, 16451148,
  31188765, 8174568, 24132030, 5838284, 8726777, 10697873, 30365043,
  40656885, 6262555, 301068, 34096955, 29442656, 36415956
};

// Move these to tests/private.c
// These are p / 4 * 3
uint64_t expected[] = {
  16635924, 23042175, 29557527, 36157857, 17908860, 24339447, 30871020,
  37483887, 19186551, 25639464, 32189226, 38816082, 20467707, 26944302,
  33509838, 40149675, 21754065, 28249914, 34829757, 41487675
};


START_TEST (MIN_find_one_generator)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN find one generator\n");
  printf ("  Calls find generator(%" PRId64 ") in a separate thread\n", large_primes[10]);
  printf ("  Should compute %" PRId64 "\n\n", large_gens[10]);

  uint64_t single = find_gen (large_primes[10]);
  ck_assert_int_eq (single, large_gens[10]);
}
END_TEST

START_TEST (MIN_find_two_generators)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN find two generators\n");
  printf ("  Calls find_generator(%" PRId64 ") in a separate thread\n", large_primes[0]);
  printf ("    Should compute %" PRId64 "\n", large_gens[0]);
  printf ("  Calls find_generator(%" PRId64 ") in a separate thread\n", large_primes[1]);
  printf ("    Should compute %" PRId64 "\n\n", large_gens[1]);

  uint64_t * pair = find_gens (2, large_primes);
  ck_assert (pair != NULL);
  ck_assert_int_eq (pair[0], large_gens[0]);
  ck_assert_int_eq (pair[1], large_gens[1]);
  free (pair);
}
END_TEST

START_TEST (MIN_find_5_generators)
{
  printf ("======================================\n");
  printf ("PRIVATE\n");
  printf ("  MIN find one generator\n");
  printf ("  Calls find generator() for the following values:\n  ");

  size_t i;
  for (i = 0; i < 5; i++)
    printf ("  %" PRId64, large_primes[i]);
  printf ("\n");
  uint64_t * gens = find_gens (5, large_primes);

  printf ("  Computed the following generators:\n  ");
  for (i = 0; i < 5; i++)
    printf ("  %" PRId64, gens[i]);
  printf ("\n");

  printf ("  Expected the following generators:\n  ");
  for (i = 0; i < 5; i++)
    printf ("  %" PRId64, large_gens[i]);
  printf ("\n\n");

  ck_assert (gens != NULL);
  for (i = 0; i < 5; i++)
    ck_assert_int_eq (gens[i], large_gens[i]);

}
END_TEST

START_TEST (MIN_last_5_discrete_logs)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN last 5 discrete logs\n");
  printf ("  Uses threads to compute the discrete logarithms for:\n");
  size_t i;
  for (i = 0; i < 5; i++)
    {
      printf ("    g = %8"PRId64 "; ", large_gens[15 + i]);
      printf ("    p = %"PRId64 "; ", large_primes[15 + i]);
      printf ("    g^n = %"PRId64 "\n", mod_powers[15 + i]);
    }

  struct time_args args;
  memset (&args, 0, sizeof (args));
  args.results = calloc (20, sizeof (uint64_t));
  args.start_index = 15;
  args.number = 5;
  args.generators = large_gens;
  args.primes = large_primes;
  args.mod_powers = mod_powers;

  pthread_t thread;
  pthread_create (&thread, NULL, time_log_thread, (void *) &args);
  pthread_join (thread, NULL);
  printf ("  Thread took %lf seconds\n", args.time_taken);
  for (i = 0; i < 5; i++)
    printf ("  Computed n = %"PRId64 " [expected %"PRId64"]\n",
            args.results[15 + i], expected[15 + i]);
  printf ("\n");
  for (i = 0; i < 5; i++)
    ck_assert_int_eq (args.results[15 + i], expected[15 + i]);
  free (args.results);

}
END_TEST

START_TEST (MIN_parallel_discrete_logs)
{
  printf ("======================================\n");
  printf ("PUBLIC\n");
  printf ("  MIN parallel discrete logs\n");
  printf ("  Uses 4 threads to compute the discrete logarithms for:\n");
  size_t i;
  for (i = 0; i < 20; i++)
    {
      printf ("    g = %8"PRId64 "; ", large_gens[15 + i]);
      printf ("    p = %"PRId64 "; ", large_primes[15 + i]);
      printf ("    g^n = %"PRId64 "\n", mod_powers[15 + i]);
    }

  uint64_t * logs = calloc (20, sizeof (uint64_t));

  struct timeval start, end;
  gettimeofday (&start, NULL);
  pthread_t thread[4];
  struct time_args args[4];
  for (i = 0; i < 4; i++)
    {
      memset (&args[i], 0, sizeof (args[i]));
      args[i].results = logs;
      args[i].start_index = i * 5;
      args[i].number = 5;
      args[i].generators = large_gens;
      args[i].primes = large_primes;
      args[i].mod_powers = mod_powers;
      pthread_create (&thread[i], NULL, time_log_thread, (void *) &args[i]);
    }

  for (i = 0; i < 4; i++)
    pthread_join (thread[i], NULL);
  gettimeofday (&end, NULL);

  double parallel_time = time_diff (start, end);
  printf ("  Parallel real-time took %lf seconds\n", parallel_time);
  for (i = 0; i < 4; i++)
    printf ("    Thread %zd took %lf seconds\n", i, args[i].time_taken);

  size_t j;
  for (i = 0; i < 4; i++)
    {
      for (j = 0; j < 5; j++)
        printf ("  Thread %zd computed n = %"PRId64 " [expected %"PRId64"]\n",
                i, logs[i * 4 + j], expected[i * 4 + j]);
    }
  printf ("\n");
  for (i = 0; i < 20; i++)
    ck_assert_int_eq (logs[i], expected[i]);
  free (logs);

  gettimeofday (&start, NULL);
  for (i = 0; i < 20; i++)
    discrete_log (mod_powers[i], large_gens[i], large_primes[i]);
  gettimeofday (&end, NULL);
  double sequential_time = time_diff (start, end);
  printf ("  Sequential real-time took %lf seconds\n", sequential_time);

  ck_assert (parallel_time * 2 < sequential_time);
  ck_assert (parallel_time * 4 > sequential_time);

}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_set_timeout (tc_public, 15.0);
  tcase_add_test (tc_public, MIN_find_one_generator);
  tcase_add_test (tc_public, MIN_find_two_generators);
  tcase_add_test (tc_public, MIN_last_5_discrete_logs);
  tcase_add_test (tc_public, MIN_parallel_discrete_logs);
  suite_add_tcase (s, tc_public);
}

