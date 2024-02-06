#ifndef __effects_h__
#define __effects_h__

#include "statemodel.h"

void reset_runtime (fsm_t *);
void incr_runtime (fsm_t *);
void set_live (fsm_t *);
void kill_proc (fsm_t *);
void say_blocked (fsm_t *);
void say_unblocked (fsm_t *);
void print_stats (fsm_t *);

#endif
