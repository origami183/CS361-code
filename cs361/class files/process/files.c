#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void dump (char *, size_t);
void run_file (char *);

int
main (int argc, char *argv[])
{
  struct stat stat;
  for (int i = 1; i < argc; i++)
    {
      int fd = open (argv[i], O_RDONLY);
      if (fd < 0)
        {
          fprintf (stderr, "ERROR: Failed to open %s\n", argv[i]);
          continue;
        }
      printf ("Opened file %s as fd %d\n", argv[i], fd);
      run_file (argv[i]);

      memset (&stat, 0, sizeof (stat));
      assert (fstat (fd, &stat) >= 0);
      printf ("\nInode     Perms   Links  UID   GID   Size\n");
      printf ("%-9ld %-7o %-6d %-5d %-5d %ld\n", stat.st_ino, stat.st_mode,
              stat.st_nlink, stat.st_uid, stat.st_gid, stat.st_size);

      char *contents = calloc (stat.st_size, sizeof (char));
      read (fd, contents, stat.st_size);
      close (fd);

      dump (contents, stat.st_size);

      if ((i + 1) < argc)
        printf ("++++++++++++++++++++++++++++++");

      printf ("\n");

    }
  return EXIT_SUCCESS;
}

void
dump (char *contents, size_t size)
{
  printf ("\nFirst part of contents:\n");
  size_t max = (size > 16 ? 16 : size);
  for (size_t i = 0; i < max; i++)
    printf ("%02x ", contents[i]);
  for (size_t i = max; i < 16; i++)
    printf ("   ");
  printf (" |");
  for (size_t i = 0; i < max; i++)
    {
      if (isprint (contents[i]))
        printf ("%c", contents[i]);
      else
        printf (".");
    }
  for (size_t i = max; i < 16; i++)
    printf (" ");
  printf ("|\n");
}

void
run_file (char *filename)
{
  pid_t pid = fork ();
  assert (pid >= 0);
  if (pid == 0)
    {
      execlp ("file", "file", filename, NULL);
      abort ();
    }
  wait (NULL);
}
