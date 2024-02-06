#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER 100

void
run_pipe (void)
{
  char *buffer = calloc (BUFFER, sizeof (char));

  int pipefd[2];
  pipe (pipefd);
  printf ("pipefd[0] = %d; pipefd[1] = %d\n", pipefd[0], pipefd[1]);

  pid_t child = fork ();
  assert (child >= 0);

  if (child == 0)
    {
      close (pipefd[1]);
      read (pipefd[0], buffer, BUFFER - 1);
      printf ("SECOND: Child read '%s'\n", buffer);
      close (pipefd[0]);
      exit (0);
    }

  close (pipefd[0]);
  printf ("FIRST: Enter a message (less than 100 characters): ");

  fflush (stdout);

  read (STDIN_FILENO, buffer, BUFFER - 1);

  // BONUS: Remove the newline?

  write (pipefd[1], buffer, BUFFER);
  wait (NULL);
  printf ("THIRD: Parent is exiting\n");
}
