#ifndef __SIGNALS_H__
#define __SIGNALS_H__

pid_t run_child (size_t, int[], char *);
char *sig2msg (int, size_t *);
int getsig (char *);

#endif
