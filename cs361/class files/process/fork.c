#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/* Preprocessor macros to generate the printf statements */
#define DUMP(name,var) printf ("[%7d %7d] %-9s 0x%08x  0x%05x  0x%x\n", getpid (), \
                               getppid (), (name), (uint32_t)&(var), \
                               (((uint32_t)&(var) & 0xfffff000) >> 12), (uint32_t)(var));
#define DUMPPTR(name,var) printf ("[%7d %7d] %-9s 0x%08x  0x%05x  0x%x\n", getpid (), \
                                  getppid (),(name), (uint32_t)(var), \
                                  (((uint32_t)(var) & 0xfffff000) >> 12), (uint32_t)*(var));


/* Create a global variable */
int global = 10;

void
run_fork (int fork_num)
{
  int local = 20;
  static int stat = 30;
  int *pointer = malloc (sizeof (int));
  assert (pointer != NULL);
  *pointer = 40;

  /* In parent process, print variable addresses and values */
  printf ("[PID     PPID   ] variable  address     page     value\n");
  DUMP ("global", global); DUMP ("local", local); DUMP ("static", stat);
  DUMP ("stackptr", pointer); DUMPPTR ("heap", pointer);
  printf ("\n");

  /* *********************************************************************** */
  /* *********************************************************************** */
  /* The code below will ONLY be executed if the -f1 flag option was passed. */
  /* *********************************************************************** */
  /* *********************************************************************** */
  if (fork_num == 1)
    {
      /* If -f1 passed, create a child process with fork() */
      pid_t pid = fork ();
      assert (pid >= 0);

      if (pid == 0)
        {
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          printf ("Child addresses after the fork...\n");
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          /* Child process: print addresses and values */
          DUMP ("global", global); DUMP ("local", local); DUMP ("static", stat);
          DUMP ("stackptr", pointer); DUMPPTR ("heap", pointer);
          printf ("\n");

          /* Now, reload the program and start over.
             DO NOT change this parameter to -f1. It will cause a fork-bomb. */
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          printf ("Child is starting over with exec...\n");
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          char *args[] = { "./procs", "-f2", NULL };
          execvp (args[0], args);
          exit (EXIT_SUCCESS);
        }

      wait (NULL);

    }

  free (pointer);
}
