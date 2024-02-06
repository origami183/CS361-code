#ifndef __CS361_MOVIES_H__
#define __CS361_MOVIES_H__

#include <stdbool.h>

typedef struct movie
{
  char *title;
  double rating;
  int runtime;
  int year;
  char *genre;
  bool nominated;
} movie_t;

movie_t split_data (char *);
char *merge_data (movie_t);

#endif