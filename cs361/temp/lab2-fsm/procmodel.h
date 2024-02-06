#ifndef __cs361_procmodel_h__
#define __cs361_procmodel_h__

#include "statemodel.h"

// Events
typedef enum
{
  ADMIT,    // admit a process into the system
  DISPATCH, // dispatch a process to run
  SCHEDULE, // move running process back to the queue
  BLOCK,    // block a process until an unblock occurs
  UNBLOCK,  // unblock a process back into the queue
  EXIT,     // running process is choosing to exit
  KILL,     // system kills non-running process
  NIL       // non-event (cause no transition)
} procevt_t;
#define NUM_EVENTS (NIL + 1)

// Possible states
typedef enum
{
  NEW, // a new process
  RDY, // ready to run
  RUN, // the (only) running process
  BLK, // blocked (waiting)
  TRM, // terminated (cannot emerge)
  NST  // invalid non-state
} procst_t;
#define NUM_STATES NST

fsm_t *process_init (void);

#endif
