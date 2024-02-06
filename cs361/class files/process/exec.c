#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void concat_ls (char *, char *, size_t);
void find_and_exec (char *, char *, size_t);
bool is_executable (char *);
void run_path (char *);
void setup_path (char **, char **, size_t *, bool);

/* Create a new process and run the new program in it */
void
run_path (char *filepath)
{
  pid_t pid = fork ();
  if (pid < 0) return;

  if (pid == 0)
    {
      execl (filepath, filepath, NULL);
      abort (); /* should never happen */
    }
  /* Parent waits until the child has exited */
  wait (NULL);
}

void
run_exec (bool malicious)
{
  char *filepath = NULL;
  char *path = NULL;
  size_t pathlen = 0;

  setup_path (&filepath, &path, &pathlen, malicious);
  assert (filepath != NULL && path != NULL);

  /* Execute "ls" with the original path */
  find_and_exec (filepath, path, pathlen);

  free (filepath);
  free (path);
}






/* **************************************************************** */
/* **************************************************************** */
/* **************************************************************** */
/* Helper files below here to improve readability of the code above */
/* **************************************************************** */
/* **************************************************************** */
/* **************************************************************** */

/* Search through the PATH for the 'ls' executable */
void
find_and_exec (char *filepath, char *path, size_t pathlen)
{
  printf ("PATH=%s\n\n", path);

  /* Start splitting the path based on the : delimiter */
  char *token = strtok (path, ":");
  while (token != NULL)
    {
      /* Append the "/ls" to the end of the current direct in the path */
      concat_ls (filepath, token, pathlen);
      printf ("Checking [%s]\n", filepath);
      if (is_executable (filepath))
        {
          printf (" --> Running %s\n", filepath);
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
          run_path (filepath);
          printf ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++\n\n");
          token = NULL;
        }
      else
        /* No file or it was not executable, so get next directory */
        token = strtok (NULL, ":");
    }
}

void
setup_path (char **filepath, char **path, size_t *pathlen, bool mal)
{
  /* Allocate space to add "/ls" onto the end of a directory */
  const char * _path = getenv ("PATH");
  *pathlen = strlen (_path) + 4;

  /* Make a copy of the path to manipulate, then a buffer to use */
  *path = malloc (*pathlen);

  if (mal)
    {
      /* Update the path to add .: to the beginning and run ls again */
      strncpy (*path, ".:", 3);
      strncat (*path, _path, *pathlen - 3);
    }
  else
    {
      strncpy (*path, _path, *pathlen);
      memset (*path + *pathlen - 4, 0, 4);
    }
  *filepath = malloc (*pathlen);
}

/* Copies token as the directory name, then appends "/ls" */
void
concat_ls (char *filepath, char *token, size_t pathlen)
{
  memset (filepath, 0, pathlen);
  assert (strlen (token) < pathlen - 3);
  strncpy (filepath, token, strlen (token) + 1);
  strncat (filepath, "/ls", pathlen - strlen (token));
}

/* Determine if the file is executable */
bool
is_executable (char *path)
{
  struct stat stats;
  if (stat (path, &stats) < 0)
    return false;

  if (S_ISREG (stats.st_mode))
    {
      if (stats.st_uid && geteuid ())
        if ((stats.st_mode & S_IXUSR) != 0) return true;

      if (stats.st_gid && getegid ())
        if ((stats.st_mode & S_IXGRP) != 0) return true;

      if ((stats.st_mode & S_IXOTH) != 0) return true;
    }

  return false;
}
