#include <assert.h>
#include <fcntl.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "signals.h"

/* Given a signal number signum, return the string and set the string length
   in the given call-by-reference parameter
     Required signals and strings:
       SIGSEGV => "SEGFAULT"
       SIGHUP  => "HANGUP"
       SIGINT  => "INTERRUPT" */
char *
sig2msg (int signum, size_t *length)
{
  assert (length != NULL);
  char *msg;
  if (signum == 11)
    {
      msg = "SEGFAULT";
      *length = 8;
    }
  else if (signum == 1)
    {
      msg = "HANGUP";
      *length = 6;
    }
  else if (signum == 2)
    {
      msg = "INTERRUPT";
      *length = 9;
    }
  return msg;
}

/* Convert the short signal name (NNN part of SIGNNN) to its number.
   Consult /usr/include/asm/signal.h for the number mappings. */
int
getsig (char *name)
{
  // TODO: Complete this function for the other signals
  if (!strncmp (name, "SEGV", 4))
    return 11; // SIGSEGV = 11
  else if (!strncmp (name, "HUP", 3))
    return 1; // SIGHUP = 1
  else if (!strncmp (name, "INT", 3))
    return 2; // SIGINT = 2
  return 0;
}