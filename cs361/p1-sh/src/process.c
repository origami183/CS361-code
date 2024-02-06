#include <fcntl.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// Our includes
#include "hash.h"
#include <assert.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#define WHITESPACE " \t\n\r"

// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe

// helper to check if a file is built in
// returns 0 if built in, 1 otherwise
int
builtInCheck (char *command)
{
  if (strncmp (command, "echo", 4) == 0 || strncmp (command, "export", 6) == 0
      || strncmp (command, "pwd", 3) == 0 || strncmp (command, "unset", 5) == 0
      || strncmp (command, "which", 5) == 0
      || strncmp (command, "quit", 4) == 0 || strncmp (command, "cd", 2) == 0)
    {
      if (strncmp (command, "which ", 6) == 0)
        {
          command[strlen (command) - 1] = '\0';
        }
      printf ("%s: dukesh built-in command\n", command);
      return 0;
    }
  return 1;
}

// helper to check if a file is a ./ executable
// return 0 if true, 1 otherwise
int
executeCheck (char *command)
{
  char temp[2];
  strncpy (temp, command, 2);
  if (strncmp (temp, "./", 2) == 0)
    {
      printf ("%s\n", command);
      return 0;
    }
  return 1;
}

int
pathCheck (char *command)
{
  // DIR *directory;
  // struct dirent *entry;
  // char *envvar = "PATH";

  char *path, *path_copy;

  path = getenv ("PATH");

  path_copy = strdup (path);

  // const char *path = getenv(envvar);
  // printf("PATH: %s\n", path_copy);

  char *token;
  token = strtok (path_copy, ":");
  struct stat buffer;

  while (token != NULL)
    {
      // strcpy(file_path, token);
      char *file;

      file = (char *)malloc (strlen (command) + strlen (token) + 2);

      strncpy (file, token, strlen (token));
      strncat (file, "/", 2);
      strncat (file, command, strlen (command));
      strncat (file, "\0", 2);

      if (stat (file, &buffer) == 0)
        {
          printf ("%s\n", file);
          free (path_copy);
          free (file);
          return 0;
        }
      token = strtok (NULL, ":");

      free (file);
    }

  free (path_copy);
  return 1;
}

int
builtIn_or_execute (char *command)
{
  if (command == NULL)
    {
      return 1;
    }
  int builtIn = builtInCheck (command);

  if (builtIn == 0)
    {
      return 0;
    }
  int execute = executeCheck (command);

  if (execute == 0)
    {
      return 0;
    }
  int pathLook = pathCheck (command);

  if (pathLook == 0)
    {
      return 0;
    }
  return 1;
}

void
tokenize_arguments (char *buffer, char ***arguments)
{
  assert (buffer != NULL);

  size_t arg_list_capacity = 10;
  size_t arg_list_length = 0;
  char **arg_array = (char **)malloc (arg_list_capacity * sizeof (char *));

  char *token = strtok (buffer, WHITESPACE);
  while (token != NULL)
    {
      if (arg_list_length + 1 == arg_list_capacity)
        {
          arg_list_capacity *= 2;
          arg_array = (char **)realloc (arg_array,
                                        arg_list_capacity * sizeof (char *));
          assert (arg_array != NULL);
        }
      arg_array[arg_list_length] = strdup (token);
      arg_list_length++;

      token = strtok (NULL, WHITESPACE);
    }
  arg_array[arg_list_length] = NULL;
  *arguments = arg_array;
}

void
cat (char *commands)
{
  char path[1024];
  memset (path, 0, sizeof (path));
  getcwd (path, sizeof (path));
  strncat (path, "/bin/cat", strlen (path) + 7);

  char **argv = NULL;
  tokenize_arguments (commands, &argv);

  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init (&action);

  int fd[2];
  pipe (fd);

  posix_spawn_file_actions_addclose (&action, fd[0]);
  posix_spawn_file_actions_adddup2 (&action, fd[1], STDOUT_FILENO);

  pid_t child;

  if (posix_spawn (&child, path, &action, NULL, argv, NULL) == 0)
    {
      close (fd[1]);

      char buffer[1024];
      size_t bytes = read (fd[0], buffer, sizeof (buffer));
      close (fd[0]);

      char *result = (char *)malloc (bytes + 1);
      strncpy (result, buffer, bytes);
      result[bytes] = '\0';

      int status;
      waitpid (child, &status, 0);
      if (status == 0)
        {
          hash_insert ("?", "0");
        }
      else
        {
          hash_insert ("?", "1");
        }
      printf ("%s", result);
      free (result);
    }
  else
    {
      perror ("Posix Spawn: cat()");
    }

  posix_spawn_file_actions_destroy (&action);

  for (size_t i = 0; argv[i] != NULL; i++)
    {
      free (argv[i]);
    }
  free (argv);
}

void
ls (char *commands)
{
  char path[1024];
  memset (path, 0, sizeof (path));
  getcwd (path, sizeof (path));
  strncat (path, "/bin/ls", strlen (path) + 7);

  char **argv = NULL;
  tokenize_arguments (commands, &argv);

  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init (&action);

  int fd[2];
  pipe (fd);

  posix_spawn_file_actions_addclose (&action, fd[0]);
  posix_spawn_file_actions_adddup2 (&action, fd[1], STDOUT_FILENO);

  pid_t child;

  if (posix_spawn (&child, path, &action, NULL, argv, NULL) == 0)
    {
      close (fd[1]);

      char buffer[1024];
      size_t bytes = read (fd[0], buffer, sizeof (buffer));
      close (fd[0]);

      char *result = (char *)malloc (bytes + 1);
      strncpy (result, buffer, bytes);
      result[bytes] = '\0';

      int status;
      waitpid (child, &status, 0);
      // store status in env hash table
      if (status == 0)
        {
          hash_insert ("?", "0");
        }
      else
        {
          hash_insert ("?", "1");
        }
      printf ("%s", result);
      free (result);
    }
  else
    {
      perror ("Posix Spawn: ls()");
    }

  posix_spawn_file_actions_destroy (&action);

  for (size_t i = 0; argv[i] != NULL; i++)
    {
      free (argv[i]);
    }
  free (argv);
}

void
head (char *commands)
{
  char path[1024];
  memset (path, 0, sizeof (path));
  getcwd (path, sizeof (path));
  strncat (path, "/bin/head", strlen (path) + 9);

  char **argv = NULL;
  tokenize_arguments (commands, &argv);

  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init (&action);

  int fd[2];
  pipe (fd);

  posix_spawn_file_actions_addclose (&action, fd[0]);
  posix_spawn_file_actions_adddup2 (&action, fd[1], STDOUT_FILENO);

  pid_t child;

  if (posix_spawn (&child, path, &action, NULL, argv, NULL) == 0)
    {
      close (fd[1]);

      char buffer[1024];
      size_t bytes = read (fd[0], buffer, sizeof (buffer));
      close (fd[0]);

      char *result = (char *)malloc (bytes + 1);
      strncpy (result, buffer, bytes);
      result[bytes] = '\0';

      int status;
      waitpid (child, &status, 0);
      // store status in env hash table
      if (status == 0)
        {
          hash_insert ("?", "0");
        }
      else
        {
          hash_insert ("?", "1");
        }
      printf ("%s", result);
      free (result);
    }
  else
    {
      perror ("Posix Spawn: head()");
    }

  posix_spawn_file_actions_destroy (&action);

  for (size_t i = 0; argv[i] != NULL; i++)
    {
      free (argv[i]);
    }
  free (argv);
}

void
tokenize_arguments2 (char *buffer, char ***arguments, char ***enviros)
{
  assert (buffer != NULL);

  int executeCharCount = 0;
  bool charCheck = false;
  bool fill = false;
  size_t arg_list_capacity = 10;
  size_t arg_list_length = 0;
  size_t env_list_length = 0;
  char **arg_array = (char **)malloc (arg_list_capacity * sizeof (char *));
  char **env_array = (char **)malloc (arg_list_capacity * sizeof (char *));

  char *token = strtok (buffer, WHITESPACE);
  while (token != NULL)
    {
      fill = false;
      if (arg_list_length + 1 == arg_list_capacity)
        {
          arg_list_capacity *= 2;
          arg_array = (char **)realloc (arg_array,
                                        arg_list_capacity * sizeof (char *));
          assert (arg_array != NULL);
        }

      if (strncmp (token, "./", 2) == 0 && charCheck == false)
        {
          if (executeCharCount == 0)
            {
              arg_array[arg_list_length] = strdup (token);
              arg_list_length++;
              fill = true;
            }
          executeCharCount++;
          if (executeCharCount == 2)
            {
              charCheck = true;
            }
        }

      if (charCheck == true)
        {
          arg_array[arg_list_length] = strdup (token);
          arg_list_length++;
        }
      if (fill == false && charCheck == false)
        {
          env_array[env_list_length] = strdup (token);
          env_list_length++;
        }

      token = strtok (NULL, WHITESPACE);
    }
  arg_array[arg_list_length] = NULL;
  env_array[env_list_length] = NULL;
  *arguments = arg_array;
  *enviros = env_array;
}

void
env (char *commands)
{
  char path[1024];
  memset (path, 0, sizeof (path));
  getcwd (path, sizeof (path));
  strncat (path, "/bin/env", strlen (path) + 8);

  char **argv = NULL;
  char **envp = NULL;
  tokenize_arguments2 (commands, &argv, &envp);

  posix_spawn_file_actions_t action;
  posix_spawn_file_actions_init (&action);

  int fd[2];
  pipe (fd);

  posix_spawn_file_actions_addclose (&action, fd[0]);
  posix_spawn_file_actions_adddup2 (&action, fd[1], STDOUT_FILENO);

  pid_t child;

  if (posix_spawn (&child, path, &action, NULL, argv, envp) == 0)
    {
      close (fd[1]);

      char buffer[1024];
      size_t bytes = read (fd[0], buffer, sizeof (buffer));
      close (fd[0]);

      char *result = (char *)malloc (bytes + 1);
      strncpy (result, buffer, bytes);
      result[bytes] = '\0';

      int status;
      waitpid (child, &status, 0);
      // store status in env hash table
      if (status == 0)
        {
          hash_insert ("?", "0");
        }
      else
        {
          hash_insert ("?", "1");
        }
      printf ("%s", result);
      free (result);
    }
  else
    {
      perror ("Posix Spawn: env()");
    }

  posix_spawn_file_actions_destroy (&action);

  for (size_t i = 0; argv[i] != NULL; i++)
    {
      free (argv[i]);
    }
  for (size_t i = 0; envp[i] != NULL; i++)
    {
      free (envp[i]);
    }
  free (envp);
  free (argv);
}