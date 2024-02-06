#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "fptr.h"
#include "movies.h"

int cmdline (int, char **, bool *, bool *, bool *);

int
add (int x, int y)
{
  return x + y;
}

int
sub (int x, int y)
{
  return x - y;
}

void
usage (void)
{
  printf ("Usage: ptrs [-m] [-a x y] [-s x y]\n");
  printf (" Options are:\n");
  printf ("  -m      Parse the CSV data as a movie\n");
  printf ("  -a x y  Add x and y using a function pointer\n");
  printf ("  -s x y  Subtract x and y using a function pointer\n");
  printf ("Only one option is allowable\n");
}

int
main (int argc, char **argv)
{
  bool run_movie = false;
  bool run_add = false;
  bool run_sub = false;

  if (cmdline (argc, argv, &run_movie, &run_add, &run_sub) != 0)
    return EXIT_FAILURE;

  if (run_movie)
    {
      // Split the following comma-separated value (CSV) data into fields
      char *data = "The Shawshank Redemption,9.3,142,1994,Drama,true";
      movie_t movie = split_data (data);
      printf ("Title: %s\n", movie.title);
      printf ("Rating: %f\n", movie.rating);
      printf ("Runtime: %d\n", movie.runtime);
      printf ("Year: %d\n", movie.year);
      printf ("Genre: %s\n", movie.genre);
      printf ("Oscar nominated? %s\n", (movie.nominated ? "yes" : "no"));

      char *rebuilt = merge_data (movie);
      printf ("%s\n", rebuilt);

      free (rebuilt);
      free (movie.title);
      free (movie.genre);

      return EXIT_SUCCESS;
    }

  if (optind != (argc - 2))
    {
      usage ();
      return EXIT_FAILURE;
    }

  int x = strtol (argv[optind], NULL, 10);
  int y = strtol (argv[optind + 1], NULL, 10);

  // Call a function that passes a function pointer
  if (run_add)
    {
      int z = run_func (x, y, add);
      printf ("%d + %d = %d\n", x, y, z);
    }
  else
    {
      int z = run_func (x, y, sub);
      printf ("%d - %d = %d\n", x, y, z);
    }

  return EXIT_SUCCESS;
}

/* DO NOT MODIFY THIS FUNCTION. Parses the command line arguments to get
   the signal numbers to override, the number of overridden signals, and
   the order of signals to raise in the full implementation. */
int
cmdline (int argc, char **argv, bool *movies, bool *add, bool *sub)
{
  int option;

  if ((option = getopt (argc, argv, "mas")) == -1)
    {
      usage ();
      return -1;
    }

  switch (option)
    {
    case 'm':
      *movies = true;
      break;
    case 'a':
      *add = true;
      break;
    case 's':
      *sub = true;
      break;
    default:
      usage ();
      return -1;
    }

  return 0;
}
