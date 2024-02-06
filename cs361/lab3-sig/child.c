#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "signals.h"

/* Signal handler for signals that are not overriden. Prints a message and
   exits. */
void
sig_unknown (int signum)
{
  write (STDOUT_FILENO, "NOT OVERRIDEN\n", 14);
  exit (EXIT_SUCCESS);
}

/* Signal handler for overriden signals. Print out the appropriate message from
   using sig2msg() to translate the signal number to a string. Note that you
   cannot use printf(). You must use write(). */
void
sig_handler (int signum)
{
  // TODO: Using write(), print out the message for the signal, then exit.
  size_t length = 1;
  char *output = sig2msg (signum, &length);
  char newStr[length + 2];
  if (signum == 11)
    {
      snprintf (newStr, sizeof (newStr), "%s\n", output);
      write (STDOUT_FILENO, newStr, length + 1);
    }
  else if (signum == 1)
    {
      snprintf (newStr, sizeof (newStr), "%s\n", output);
      write (STDOUT_FILENO, newStr, length + 1);
    }
  else if (signum == 2)
    {
      snprintf (newStr, sizeof (newStr), "%s\n", output);
      write (STDOUT_FILENO, newStr, length + 1);
    }
  exit (EXIT_SUCCESS);
}

/* Create a child and override the signals listed in the array.
   The parent should return the child PID; child infinitely loops
   after overriding the signal handlers. */
pid_t
run_child (size_t number_of_signals, int signals[], char *semname)
{
  // Do NOT modify these three lines, which are necessary for the timing to
  // work properly.
  sem_unlink (semname);
  sem_t *start = sem_open (semname, O_CREAT, 0644, 0);
  if (start == NULL)
    return EXIT_FAILURE;

  // Create a child process. In that child process, first use sigaction()
  // to override the default signal handlers to use sig_unknown(). Then,
  // use sigaction() again to override the signals specified on the command
  // line to use sig_handler().
  //
  // NOTE: We are only dealing with SEGV, HUP, and INT. Only these three
  // signals should be overriden.
  //
  int curSignal = 0;
  pid_t child = fork ();
  if (child == 0)
    {
      struct sigaction sigAct;
      memset (&sigAct, 0, sizeof (sigAct));
      for (int i = 0; i < number_of_signals; i++)
        {
          curSignal = signals[i];
          // printf ("%ld\n", number_of_signals);
          sigAct.sa_handler = sig_unknown;
          if (sigaction (curSignal, &sigAct, NULL) == -1)
            {
              write (STDOUT_FILENO, "NOT OVERRIDEN\n", 14);
            }
          sigAct.sa_handler = sig_handler;
          if (sigaction (curSignal, &sigAct, NULL) == -1)
            {
              write (STDOUT_FILENO, "NOT OVERRIDEN\n", 14);
            }
        }
      sem_post (start);
      while (1)
        ;
    }
  // CRITICAL: The code for the child process should end with these two
  // lines of code. The will let the parent know that the child has started,
  // then enter an infinite loop until the signal arrives. The child should
  // not do anything after this code.
  //

  // Do NOT modify these three lines. These are the parent's code. It must
  // wait until the child starts before returning.
  sem_wait (start);
  sem_unlink (semname);
  return child;
}