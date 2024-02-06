#include <fcntl.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pipe.h"

/* Given a writable string buffer, find the first space and replace it
   with a '\0', thus ending the string at that location. Sample call:

     char buffer[] = "hello world";
     char *result = split_string (buffer);
     // result points to buffer, which prints as "hello"
   */
char *
split_string (char *buffer)
{
  char *space;
  space = strchr (buffer, ' ');
  if (space != NULL)
    {
      *space = '\0';
      space = strchr (space, ' ');
    }
  return buffer;
}

/* Creates a child process. If this fails, close the pipe and return NULL.
   If the child is created, use it to run the cksum program on the filename
   passed. Send the result back through the pipe. The parent process should
   just return the child's PID. Sample call:

     int fd[2];
     pipe (fd);
     pid_t child_pid = create_cksum_child (fd, "foo.txt");
  */
pid_t
create_cksum_child (int *pipeNum, char *const filename)
{
  int des = open (filename, O_RDWR);
  if (des < 0)
    {
      return 0;
    }
  if (pipe (pipeNum) < 0)
    {
      return -1;
    }

  pid_t child = fork ();
  if (child == -1)
    {
      close (pipeNum[0]);
      close (pipeNum[1]);
      return -1;
    }
  close (des);
  if (child == 0)
    {
      close (pipeNum[0]);
      dup2 (pipeNum[1], STDOUT_FILENO);
      execlp ("cksum", "cksum", filename, NULL);
      close (pipeNum[1]);
      return -1;
    }

  return child;
}

/* Uses the cksum program on the input filename. Start by creating a pipe
   and calling create_cksum_child() to create the child process. If that
   fails, return NULL. Otherwise, implement the parent here so that it
   reads the result of the cksum calculation from the pipe and returns
   the result. Sample call:

     char *sum = get_cksum ("data/f1.txt");
     // sum is "3015617425 6 data/f1.txt\n" [with the newline]
   */
char *
get_cksum (char *const filename)
{
  int pipeNum[2];
  pipe (pipeNum);
  pid_t child = create_cksum_child (pipeNum, filename);

  if (child == -1)
    {
      close (pipeNum[0]);
      close (pipeNum[1]);
      return NULL;
    }

  char *str = calloc (100, sizeof (char *));
  int strLength = 15 + strlen (filename);

  if (child != 0)
    {
      read (pipeNum[0], str, strLength);
    }

  char *returnStr = strndup (str, strLength);
  free (str);
  str = NULL;
  return returnStr;
}

/* Re-implementation of the create_cksum_child() in pipe.c. Instead of
   using the calls to pipe/fork/dup2/exec, combine the latter three
   into a call to posix_spawn(). Sample call:

     char *sum = spawn_cksum ("data/f1.txt");
     // sum is "3015617425 6 data/f1.txt\n" [with the newline]
   */
char *
spawn_cksum (char *const filename)
{
  // Use these parameters to posix_spawn(). You will need to set up
  // the posix_spawn_file_actions_t by (1) initializing it, (2) adding
  // a close action, and (3) adding a dup2 action so that STDOUT writes
  // to a pipe. After spawning the new process, make sure to call
  // posix_spawn_file_actions_destroy() on the actions to prevent
  // memory leaks.

  const char *path = "/usr/bin/cksum";
  char *const argv[] = { "cksum", filename, NULL };

  // Instead of using fork() and exec(), use the posix_spawn functions.
  // Note that you will need to add a close and dup2 action to the
  // set of file actions to run when spawning. You will also need to
  // create the pipe to use with the dup2 action.

  // Parent code: read the value back from the pipe into a dynamically
  // allocated buffer. Wait for the child to exit, then return the
  // buffer.
  int pipeNum[2];
  pipe (pipeNum);
  pid_t child = -1;
  int fileDes = open (filename, O_RDWR);

  if (0 <= fileDes)
    {
      posix_spawn_file_actions_t action;
      posix_spawn_file_actions_init (&action);
      posix_spawn_file_actions_addclose (&action, pipeNum[0]);
      posix_spawn_file_actions_adddup2 (&action, pipeNum[1], STDOUT_FILENO);
      posix_spawn (&child, path, &action, NULL, argv, NULL);
      wait (NULL);
      posix_spawn_file_actions_destroy (&action);
    }

  char *str = calloc (100, sizeof (char *));
  int strLength = 15 + strlen (filename);

  if (0 <= fileDes)
    {
      read (pipeNum[0], str, strLength);
    }

  char *returnStr = strndup (str, strLength);
  free (str);
  str = NULL;
  return returnStr;
}
