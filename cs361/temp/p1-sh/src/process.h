#ifndef __cs361_process__
#define __cs361_process__

// The contents of this file are up to you, but they should be related to
// running separate processes. It is recommended that you have functions
// for:
//   - performing a $PATH lookup
//   - determining if a command is a built-in or executable
//   - running a single command in a second process
//   - running a pair of commands that are connected with a pipe

int builtInCheck (char *);
int executeCheck (char *);
int pathCheck (char *);
int builtIn_or_execute (char *);
void tokenize_arguments (char *, char ***);
void tokenize_arguments2 (char *, char ***, char ***);
void cat (char *);
void ls (char *);
void head (char *);
void env (char *);

#endif
