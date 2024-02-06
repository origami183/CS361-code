#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

void
run_dup (int old, int new)
{
  const char * const names[] = { "", "STDOUT", "STDERR" };
  printf ("STDOUT: Merging %d (%s) into %d (%s)\n", new, names[new],
          old, names[old]);

  close (new);
  dup2 (old, new);
  fprintf (stderr, "STDERR: Hello\n");
  printf ("STDOUT: Hello\n");
}
