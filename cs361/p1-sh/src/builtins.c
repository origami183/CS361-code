#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "hash.h"
#include "process.h"

// Given a message as input, print it to the screen followed by a
// newline ('\n'). If the message contains the two-byte escape sequence
// "\\n", print a newline '\n' instead. No other escape sequence is
// allowed. If the sequence contains a '$', it must be an environment
// variable or the return code variable ("$?"). Environment variable
// names must be wrapped in curly braces (e.g., ${PATH}).
//
// Returns 0 for success, 1 for errors (invalid escape sequence or no
// curly braces around environment variables).
int
echo (char *message)
{
  if (message != NULL) // add null char to end of the message
    {
      message[strlen (message) - 1] = '\0';
    }

  char *token = strtok (message, "\\n"); // tokenize message
  while (token != NULL)
    {
      char *start = strchr (token, '$'); // pull text for environment print
      if (start != NULL
          && start[1] == '{') // hashed in environment check (non $?)
        {
          char *end
              = strchr (start, '}'); // pull everything until closing bracket
          if (end == NULL)
            {
              return 1;
            }

          int varLength = end - start - 2;
          char var[varLength + 1];
          strncpy (var, start + 2, varLength);
          var[varLength] = '\0';

          char *val = hash_find (var); // pull value from the hash table

          int before = strlen (token) - strlen (start);
          if (val != NULL) // if value found from hash table
            {
              if (before
                  >= 2) // grabs everything before {} and the value in {}
                {
                  char buffer[before + 1];
                  strncpy (buffer, token, before);
                  buffer[before] = '\0';
                  printf ("%s%s\n", buffer, val);
                  return 0;
                }
              printf ("%s\n", val);
              return 0;
            }
          else // prints everything before {}
            {
              char buffer[before + 1];
              strncpy (buffer, token, before);
              buffer[before] = '\0';
              printf ("%s\n", buffer);
              return 0;
            }

          start = end + 1; // moves start pointer
        }
      else if (start != NULL
               && start[1] == '?') // Looksup the ? symbol to the hastable
        {
          char *env = hash_find ("?");
          printf ("%s\n", env);
          fflush (stdout);
          return 0;
        }
      else // non-enviromental variable text echo
        {
          printf ("%s\n", token);
          token = strtok (NULL, "\\n");
        }
    }

  return 0;
}

// Given a key-value pair string (e.g., "alpha=beta"), insert the mapping
// into the global hash table (hash_insert ("alpha", "beta")).
//
// Returns 0 on success, 1 for an invalid pair string (kvpair is NULL or
// there is no '=' in the string).
int export(char *kvpair)
{
  if (kvpair == NULL
      || strchr (kvpair, '=') == NULL) // is either the key or '=' non-existant
    {
      return 1;
    }

  char *key = strtok (kvpair, "="); // get the key
  char *val = strtok (NULL, "\\n"); // get the value

  val[strlen (val) - 1] = '\0'; // add null char to value

  hash_insert (key, val); // add kvpair to the hash
  return 0;
}

// Prints the current working directory (see getcwd()). Returns 0.
int
pwd (void)
{
  char curDirect[1024]; // buffer to store cur direct.

  getcwd (curDirect, sizeof (curDirect));
  printf ("%s\n", curDirect);
  return 0;
}

// Removes a key-value pair from the global hash table.
// Returns 0 on success, 1 if the key does not exist.
int
unset (char *key)
{
  return hash_remove (key);
}

// Given a string of commands, find their location(s) in the $PATH global
// variable. If the string begins with "-a", print all locations, not just
// the first one.
//
// Returns 0 if at least one location is found, 1 if no commands were
// passed or no locations found.
int
which (char *cmdline)
{
  if (cmdline != NULL) // does the command exist?
    {
      cmdline[strlen (cmdline) - 1] = '\0'; // add null char
    }
  int locationFound = 0; // improvised bool, 1 = false, 0 = true

  locationFound = builtIn_or_execute (
      cmdline); // pass to process.c to determine location
  return locationFound;
}
