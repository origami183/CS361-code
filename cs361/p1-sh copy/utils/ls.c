//Used for basic input/output stream
#include <stdio.h>
//Used for handling directory files
#include <dirent.h>
//For EXIT codes and error handling
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <spawn.h>

// System
#include <sys/stat.h>
#include <sys/wait.h>

static void usage (void);
static int get_arguments(int *op_a, int *op_s, char *argv[]);
static void _ls (const char *, int, int);
static void _lsPipe (const char *, int, int, int);

int
main (int argc, char *argv[], char *envp[])
{
  bool pipeNeed = false; 

	if (false)
		{
		usage ();
		}

	int op_a = 0;
	int op_s = 0;

	int lsIndex = 0;

	for (int i = 0; argv[i] != NULL; i++)
		{
		if (strncmp (argv[i], "|", 1) == 0)
			{
				pipeNeed = true;
				break;
			}
			lsIndex++;
		}

	if (pipeNeed == false)
    {
      if (lsIndex == 1)
        {
          _ls (".",0,0);
        }
      else if (lsIndex == 2)
        {
          int directive = get_arguments (&op_a, &op_s, argv);
          switch (directive)
          {
            // No directory passed
            case 0:
              {
                _ls (".",op_a,op_s);
              }
            // Directory passed
            case 1:
              {
                char buffer[1024];
                buffer[0] =	'.';
                buffer[1] = '/';
                strncpy(buffer + 2, argv[1], strlen (argv[1]));
                _ls(buffer, op_a, op_s);
              }
          }
        } 
      else if (lsIndex == 3)
        {
          // Directory and arguments passed
          if (get_arguments(&op_a, &op_s, argv) == 0)
            {
              char buffer[1024];
              buffer[0] =	'.';
              buffer[1] = '/';
              strncpy (buffer + 2, argv[2], strlen (argv[2]));
              _ls (buffer ,op_a,op_s);
            }
          else 
            {
              usage (); 
              exit (EXIT_FAILURE);	
            }
        }
    }

	if (pipeNeed == true)
		{
			char path[1024];
			memset (path, 0, sizeof (path));
			getcwd (path, sizeof (path));
			strncat (path, "/bin/head", strlen (path) + 9);

			char **argvPass = (char **)malloc (5 * sizeof (char *));
			argvPass[0] = "./bin/head";

			if (argv[5] != NULL && argv[6] != NULL)
				{
					argvPass[1] = argv[5];
					argvPass[2] = argv[6];
				}

			int pipeNum[2];
			pipe (pipeNum);

      if (lsIndex == 1)
        {
          _lsPipe (".",0,0, pipeNum[1]);
        }
      else if (lsIndex == 2)
        {
          int directive = get_arguments (&op_a, &op_s, argv);
          switch (directive)
          {
            // No directory passed
            case 0: 
              {
                _lsPipe(".",op_a,op_s, pipeNum[1]);
              }
            // Directory passed
            case 1:
              {
                char buffer[1024];
                buffer[0] =	'.';
                buffer[1] = '/';
                strncpy (buffer + 2, argv[1], strlen (argv[1]));
                _lsPipe (buffer, op_a, op_s, pipeNum[1]);
              }
          }
        } 
      else if (lsIndex == 3)
        {
          // Directory and arguments passed
          if (get_arguments (&op_a, &op_s, argv) == 0)
            {
              char buffer[1024];
              buffer[0] =	'.';
              buffer[1] = '/';
              strncpy (buffer + 2, argv[2], strlen (argv[2]));
              _lsPipe (buffer ,op_a,op_s, pipeNum[1]);
            }
          else 
            {
              usage (); 
              return EXIT_FAILURE;	
            }
        }
      write (pipeNum[1], "end\n", 4);

			posix_spawn_file_actions_t action;
			posix_spawn_file_actions_init (&action);
			posix_spawn_file_actions_addclose (&action, pipeNum[1]);
			posix_spawn_file_actions_adddup2 (&action, pipeNum[0], STDIN_FILENO);

			pid_t child;

			if (posix_spawn (&child, path, &action, NULL, argvPass, NULL) == 0)
				{
				wait (NULL);
				}

			close (pipeNum[1]);
			close (pipeNum[0]);
		}
	return EXIT_SUCCESS;
}

int
get_arguments (int *op_a, int *op_s, char *argv[])
{
	if (argv[1][0] == '-')
		{
			char *p = (char*)(argv[1] + 1);
      // sets op_a and op_s
			while (*p)
        {
				  if (*p == 'a') 
            {
              *op_a = 1;
            }
				else if (*p == 's') 
          {
					  *op_s = 1;
				  }
				else 
          {
					  return -1;
				  }
				p++;
			}
			return 0;
		}
	return 1;
}

void _ls (const char *dir,int op_a,int op_s)
{
	struct dirent *d;
	DIR *dh = opendir (dir);
	if (!dh)
    {
      exit (EXIT_FAILURE);
    }
	while ((d = readdir (dh)) != NULL)
	{
    // Option s: File size
		if (op_s) 
      {
        struct stat file_info;
        char path[1024];
        snprintf (path, sizeof(path), "%s/%s", dir, d->d_name);

        // Grabs file size    
        if (stat (path, &file_info) == 0 && S_ISREG(file_info.st_mode)) 
          {
            if (!op_a && d->d_name[0] == '.')
              {
                continue;
              }
            else
              {
                printf ("%lld ", (long long)file_info.st_size);
              }
          } 
        else 
          {
            // Print file size for hidden directory
            if (op_a)
              {
                printf ("4096 ");
              }
          }
        }

    // Option a: Hidden files
    if (op_a) 
      {
        printf ("%s\n", d->d_name);
      }
    else 
      {
        if (d->d_name[0] != '.') 
          {	
            printf ("%s\n", d->d_name); 
          }
      }
	}
}

void _lsPipe (const char *dir, int op_a, int op_s, int pipe)
{
	struct dirent *d;
	DIR *dh = opendir(dir);
	if (!dh)
    {
      exit (EXIT_FAILURE);
    }
  // int written = 1;
	while ((d = readdir (dh)) != NULL)
    {
      char str[1024];
      memset (str, 0, sizeof (str));
      
      // Option s: File size
      if (op_s)
        {
          struct stat file_info;
          char path[1024];
          snprintf (path, sizeof (path), "%s/%s", dir, d->d_name);
              
          if (stat (path, &file_info) == 0 && S_ISREG (file_info.st_mode)) 
            {
              if (!op_a && d->d_name[0] == '.')
                {
                  continue;
                }
              else
                {
                  sprintf (str, "%lld ", (long long)file_info.st_size);
                }
            }
          else
            {
              // File size of hidden directory
              if (op_a)
                {
                  strncat (str, "4096 ", 6);
                }
            }
        }
      // Option a: Hidden directory
      if (op_a)
        {
          strncat (str, d->d_name, strlen (d->d_name));
          strncat (str, "\n", 2);
        }
      else
        {
          if (d->d_name[0] != '.') 
            {	
              strncat (str, d->d_name, strlen (d->d_name)); 
              strncat (str, "\n", 2);
            }
        }
      write (pipe, str, strlen (str));
      str[0] = '\0';
    }
}

static void
usage (void)
{
  printf ("ls, list directory contents\n");
  printf ("usage: ls [FLAG ...] [DIR]\n");
  printf ("FLAG is one or more of:\n");
  printf ("  -a       list all files (even hidden ones)\n");
  printf ("  -s       list file sizes\n");
  printf ("If no DIR specified, list current directory contents\n");
}
