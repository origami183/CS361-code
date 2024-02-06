#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <check.h>

#include "../fptr.h"
#include "../movies.h"

// Test that split_data gets the title copy
START_TEST (UNIT_split_title)
{
  char *csv = "Casablanca,8.5,102,1942,Romance,true";
  movie_t movie = split_data (csv);
  ck_assert_str_eq (csv, "Casablanca,8.5,102,1942,Romance,true");
  ck_assert_str_eq (movie.title, "Casablanca");
}
END_TEST

// Test that split_data parses numeric values
START_TEST (UNIT_split_nums)
{
  char *csv = "Casablanca,8.5,102,1942,Romance,true";
  movie_t movie = split_data (csv);
  ck_assert_str_eq (csv, "Casablanca,8.5,102,1942,Romance,true");
  //ck_assert_float_eq_tol (movie.rating, 8.5, 0.1);
  ck_assert (fabs (movie.rating - 8.5) < 0.1);
  ck_assert_int_eq (movie.runtime, 102);
  ck_assert_int_eq (movie.year, 1942);
}
END_TEST

// Test that split_data gets the genre copy
START_TEST (UNIT_split_genre)
{
  char *csv = "Casablanca,8.5,102,1942,Romance,true";
  movie_t movie = split_data (csv);
  ck_assert_str_eq (csv, "Casablanca,8.5,102,1942,Romance,true");
  ck_assert_str_eq (movie.genre, "Romance");
}
END_TEST

// Test that split_data parses boolean
START_TEST (UNIT_split_bool)
{
  char *csv = "Casablanca,8.5,102,1942,Romance,true";
  movie_t movie = split_data (csv);
  ck_assert_str_eq (csv, "Casablanca,8.5,102,1942,Romance,true");
  ck_assert (movie.nominated);

  csv = "Boomerang,5.4,117,1992,Comedy,false";
  movie = split_data (csv);
  ck_assert_str_eq (csv, "Boomerang,5.4,117,1992,Comedy,false");
  ck_assert (!movie.nominated);
}
END_TEST

// Test that merge_data combines the fields into a string
START_TEST (UNIT_merge_movie)
{
  movie_t movie;
  movie.title = "The Lord of the Rings: The Fellowship of the Ring";
  movie.year = 2001;
  movie.runtime = 178;
  char *result = merge_data (movie);
  ck_assert_str_eq (result, "[2001] The Lord of the Rings: The Fellowship of the Ring - 178 minutes");
  free (result);
}
END_TEST

int
bitmask (int mask, int bit)
{
  return (mask & bit) != 0;
}

// Test passing bitmask to run_func
START_TEST (UNIT_bitmask)
{
  int mask = 0xc;
  ck_assert_int_eq (run_func (mask, 1, bitmask), 0);
  ck_assert_int_eq (run_func (mask, 2, bitmask), 0);
  ck_assert_int_eq (run_func (mask, 4, bitmask), 1);
  ck_assert_int_eq (run_func (mask, 8, bitmask), 1);
  ck_assert_int_eq (run_func (mask, 16, bitmask), 0);
}
END_TEST

void public_tests (Suite *s)
{
  TCase *tc_public = tcase_create ("Public");
  tcase_add_test (tc_public, UNIT_split_title);
  tcase_add_test (tc_public, UNIT_split_nums);
  tcase_add_test (tc_public, UNIT_split_genre);
  tcase_add_test (tc_public, UNIT_split_bool);
  tcase_add_test (tc_public, UNIT_merge_movie);
  tcase_add_test (tc_public, UNIT_bitmask);
  suite_add_tcase (s, tc_public);
}

