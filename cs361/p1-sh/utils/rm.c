#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void usage (void);

int
main (int argc, char *argv[])
{
  if (false)
    {
      usage ();
    } 
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("rm, remove a file\n");
  printf ("usage: rm FILE\n");
}
