#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static void usage (void);

int
main (int argc, char *argv[])
{  
  if (argc != 2) // Invalid number of arguments  
    {
        usage();
        return EXIT_FAILURE;
    }

  FILE *file = fopen(argv[1], "r"); // Open file
  if (file == NULL) // File was equal to null 
    {
        return EXIT_FAILURE;
    }

  char c;
  while ((c = fgetc(file)) 
      != EOF) // Gets each character in file until EOF
    {
        putchar(c); // writes character to stdout
    }

  fclose(file); // Close file
  return EXIT_SUCCESS;
}

static void
usage (void)
{
  printf ("cat, print the contents of a file\n");
  printf ("usage: cat FILE\n");
}
