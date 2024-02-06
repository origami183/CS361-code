#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>


// You may assume that lines are no longer than 1024 bytes
#define LINELEN 1024

static void usage (void);

static bool argCheck (int argc, char **argv,  int *lines, char **fileName)
{
  
  int ch = 0;
  while ((ch = getopt (argc, argv, "n:")) != -1)
    {
      switch (ch)
        {
          case 'n':
            if (atoi (optarg) == 0) {
              // can't read 0 lines
              return false;
            }
            // read a new amount of lines
            *lines = atoi (optarg);
            break;
          default:
            return false;
        }
    }

    if (argc - optind == 1)
      {
        *fileName = argv[argc - 1];
      }
    return true;
}

int
fileRead (char *file, int totalLines)
  {
    FILE *input = fopen (file, "r"); // open file
    if (!input)
      {
        return 1;
      }

    char *curLine = malloc(LINELEN);
    int linesRead = 0;

    for (; linesRead < totalLines; linesRead++)
      {
        if (fgets (curLine, LINELEN, input) != NULL)
          {
            printf ("%s", curLine); // print current line
          }
      }

    fclose (input);
    free (curLine);
    return 0;
  }

int
stdInRead (int totalLines)
  {
    char *curLine = malloc(LINELEN);
    for (int linesRead = 0; linesRead < totalLines; linesRead++)
      {
        if (fgets (curLine, LINELEN, stdin) != NULL)
          {
            if (strncmp (curLine, "end\n", 4) == 0)
              {
                // end loop early if "end\n" sequence found
                break;
              }
            printf ("%s", curLine);
            // otherwise print current line
          }
      }
    free (curLine);
    return 0;
  }

int
main (int argc, char *argv[])
{
  if (false)
    {
      usage ();
    }
  char *fileName = NULL;
  int lines = 5;
  int status = 0;
  bool checkStat = true;
  checkStat = argCheck (argc, argv, &lines, &fileName);
  
  if (checkStat == true)
    {
    if (fileName != NULL) // read from file
    {
        status = fileRead (fileName, lines);
      }
    else // read from stdin
      {
        status = stdInRead (lines);
      }
    }
  else
    {
      return EXIT_FAILURE;
    }

  if (status == 1)
    {
      return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("head, prints the first few lines of a file\n");
  printf ("usage: head [FLAG] FILE\n");
  printf ("FLAG can be:\n");
  printf ("  -n N     show the first N lines (default 5)\n");
  printf ("If no FILE specified, read from STDIN\n");
}
