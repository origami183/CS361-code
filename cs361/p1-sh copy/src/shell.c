#include <assert.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#include "builtins.h"
#include "hash.h"
#include "process.h"
#include <unistd.h>

#define WHITESPACE " \t\n\r"

// No command line can be more than 100 characters
#define MAXLENGTH 100
// char **tokenized_arguments (char *);

void
shell (FILE *input)
{
  hash_init (100);
  hash_insert ("?", "0");
  char buffer[MAXLENGTH];

  char **args = calloc (10, sizeof (char *));

  // char cdPATH[MAXLENGTH]; // array to store new directory name
  // int numOfLoops = 0; // variable to track number of times the while loop
  // runs
  while (1)
    {
      // Print the cursor and get the next command entered
      printf ("$ ");
      memset (buffer, 0, sizeof (buffer));

      if (fgets (buffer, MAXLENGTH, input) == NULL)
        break;

      if (input != stdin)
        printf ("%s", buffer);

      if (strncmp (buffer, "echo", 4) == 0) // echo "echo"
        {
          int status;
          status = echo (&buffer[5]);
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "cd", 2) == 0) // change directory "cd"
        {
          int status;
          char *temp = &buffer[3];
          status = temp[strlen (temp) - 1] = '\0';
          chdir (temp);
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "which", 5)
          == 0) // source of given argument "which"
        {
          int status;
          status = which (&buffer[6]);
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "pwd", 3) == 0) // print cur directory "pwd"
        {
          int status;
          status = pwd ();
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "export", 6) == 0) // export
        {
          int status;
          status = export(&buffer[7]);
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "unset", 5) == 0) // unset
        {
          int status;
          status = unset (&buffer[6]);
          if (status == 0)
            {
              hash_insert ("?", "0");
            }
          else
            {
              hash_insert ("?", "1");
            }
        }

      if (strncmp (buffer, "quit", 4) == 0) // end the shell "quit"
        {
          break;
        }
      if (strncmp (buffer, "./bin/ls", 8)
          == 0) // list files in given directory "ls"
        {
          ls (buffer);
        }
      if (strncmp (buffer, "./bin/head", 10)
          == 0) // print some number of lines from file/input "head"
        {
          head (buffer);
        }
      if (strncmp (buffer, "./bin/env", 9)
          == 0) // environmental variables "env"
        {
          env (buffer);
        }
      if (strncmp (buffer, "./bin/cat", 9)
          == 0) // environmental variables "cat"
        {
          cat (buffer);
        }
      // numOfLoops++;
    }
  free (args);
  printf ("\n");
  hash_destroy ();
}
