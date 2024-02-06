#ifndef __pipe_h__
#define __pipe_h__

#include <unistd.h>

#define BUFFER_LENGTH 100

char *split_string (char *);
pid_t create_cksum_child (int *, char *const);
char *get_cksum (char *const);
char *spawn_cksum (char *const);

#endif
