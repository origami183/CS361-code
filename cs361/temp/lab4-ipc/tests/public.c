#include <check.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../pipe.h"

// Change this to true if you want to inspect the files produced in
// tests/inputs for debugging
#define DEBUG false

START_TEST (MIN_split_string)
{
  printf ("\n======================================\n");
  printf ("MIN_split_string\n");
  printf ("Splits \"ha ha\" into \"ha\"\n");

  char buffer[] = "ha ha";
  char *result = split_string (buffer);
  ck_assert_str_eq (result, "ha");
}
END_TEST

START_TEST (MIN_cksum)
{
  printf ("\n======================================\n");
  printf ("MIN_cksum\n");
  printf ("Gets cksum of data/f1.txt\n");

  char *result = get_cksum ("data/f1.txt");
  ck_assert_str_eq (result, "3015617425 6 data/f1.txt\n");
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, MIN_split_string);
  tcase_add_test (tc_public, MIN_cksum);
  suite_add_tcase (s, tc_public);
}

