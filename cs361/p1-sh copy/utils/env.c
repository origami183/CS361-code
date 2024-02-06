#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

static void usage (void);

int
main (int argc, char *argv[], char *envp[])
{
  bool pipeNeed = false; // Bool for pipe
  
  if (false)
    {
      usage ();
    }
  for (int i = 0; argv[i]
      != NULL; i++) // Checks argv for "|" character
    {
      if (strncmp (argv[i], "|", 1) == 0)
        {
          pipeNeed = true;
        }
    }
  if ((argv[1] != NULL 
      || argv[0] != NULL) && pipeNeed == false) // No pipe needed 
    {
      for (int i = 0; envp[i] != NULL; i++)
        {
          printf ("%s\n", envp[i]);
        }
    }

  if (pipeNeed == true) // Pipe needed
    {
      char path[1024];
      memset (path, 0, sizeof (path));
      getcwd (path, sizeof (path));
      strncat (path, "/bin/head", strlen (path) + 9); // Path of head

      char **argvPass = (char **)malloc (5 * sizeof (char *));
      argvPass[0] = "./bin/head";
      if (argv[4] != NULL 
          && argv[5] != NULL) // sets argv for head
        {
          argvPass[1] = argv[4];
          argvPass[2] = argv[5];
        }

      int pipeNum[2];
      pipe (pipeNum);

      for (int i = 0; envp[i]
          != NULL; i++) // Writing enviroment variables into the pipe
        {
          char *str = calloc (100, sizeof (char *));
          strncat (str, envp[i], strlen (envp[i]));
          strncat (str, "\n", 2);
          write (pipeNum[1], str, strlen (str));
          free (str);
        }

      posix_spawn_file_actions_t action;
      posix_spawn_file_actions_init (&action);
      posix_spawn_file_actions_addclose(&action, pipeNum[1]);
      posix_spawn_file_actions_adddup2(&action, pipeNum[0], STDIN_FILENO);

      pid_t child;

      if (posix_spawn (&child, path, &action, NULL, argvPass, NULL) == 0)
        {
          wait (NULL);
        }

      close (pipeNum[1]);
      close (pipeNum[0]);
      free (argvPass);
      
      posix_spawn_file_actions_destroy (&action);
    }
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("env, set environment variables and execute program\n");
  printf ("usage: env [name=value ...] PROG ARGS\n");
}

