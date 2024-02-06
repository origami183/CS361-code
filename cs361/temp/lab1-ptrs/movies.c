#define _POSIX_C_SOURCE 200809L // needed for strdup extension

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movies.h"

// Given a string (possibly read-only) of IMDB movie data
// (formatted as "Title,Rating,Runtime,Year,Genre,OscarNominated"),
// create a movie_t object based on the appropriate fields.
movie_t
split_data (char *csv)
{
  movie_t movie;
  char *csvWrite = strdup (csv);
  char *token = strtok (csvWrite, ",");
  movie.title = strdup (token); // copy the first token as the title

  // Use strtok() repeatedly to get each string one at a time and format
  // the data fields as follows:

  //   Use strtof() to parse the next token as a float for the rating
  // printf("%s\n", csv);
  token = strtok (NULL, ",");
  movie.rating = strtof (token, NULL); // rating token

  //   Use strtol() to get the next two tokens as runtime and year
  token = strtok (NULL, ",");
  movie.runtime = strtol (token, NULL, 10); // runtime token
  token = strtok (NULL, ",");
  movie.year = strtol (token, NULL, 10); // year token

  //   Copy the token as the movie's genre field
  token = strtok (NULL, ",");
  movie.genre = strdup (token); // genre token

  //   If the last token is equal to "true", set the nominated field to true
  token = strtok (NULL, ",");
  if (strncmp (token, "true", 4) == 0) // nominated token
    {
      movie.nominated = true;
    }
  else
    {
      movie.nominated = false;
    }

  return movie;
}

// Build a dynamically allocated string from a movie_t object as follows:
// "[Year] Title - Runtime minutes"
// Rather than allocating all of the space ahead of time, use realloc()
// to grow the string at different stages as needed.
char *
merge_data (movie_t movie)
{
  // Initially an empty string with just enough space for "[Year] " and
  // write the year into this space.
  char *result = calloc (8, sizeof (char *));
  int size = snprintf (result, 8, "[%d] ", movie.year);

  // Now, use realloc() to grow the string with enough space for the
  // title, then copy the title into the new space.
  int titleLen = strlen (movie.title) + 4;
  result = (char *)realloc (result, size + titleLen);
  size = size + snprintf (result + size, titleLen, "%s - ", movie.title);

  // Finally, allocate enough space for the rest of the line and write
  // it in using snprintf(). You may assume the runtime is at most 3 characters
  // long.
  result = (char *)realloc (result, size + 13);
  snprintf (result + size, size + 13, "%d minutes", movie.runtime);

  return result;
}