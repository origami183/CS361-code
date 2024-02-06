#ifndef __pthread_h__
#define __pthread_h__

#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

uint64_t find_gen (uint64_t);
uint64_t * find_gens (size_t, uint64_t *);

// struct used for passing arguments to time_log_thread ()
struct time_args {
  size_t start_index;     // starting index in the array of 20 items
  size_t number;          // how many items to compute
  double time_taken;      // how long total to compute all of the items
  uint64_t * generators;  // pointer to array of group generators
	uint64_t * primes;      // pointer to array of primes to use
	uint64_t * mod_powers;  // pointer to array of g^n mod p values
  uint64_t * results;     // pointer to array to capture the results
};

double time_diff (struct timeval, struct timeval);
void * time_log_thread (void *);

#endif
