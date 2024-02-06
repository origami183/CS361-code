#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "discrete.h"
#include "private.h"
#include "pthread.h"

/* Create a thread that runs generator(prime). Return the result that is
   retrieved with pthread_join().
 */
uint64_t
find_gen (uint64_t prime)
{
  pthread_t thread;
  uint64_t result = 0;
  
  pthread_create (&thread, NULL, generator, (void *) prime);
  pthread_join (thread, (void *) &result);

  return result;
}

/* Create multiple threads, each of which makes a single call to generator().
   Return a dynamically allocated array structured like the following:
     results[0] = generator (0);
     results[1] = generator (1);
     ...
 */
uint64_t *
find_gens (size_t num_threads, uint64_t * primes)
{
  pthread_t threads[num_threads];

  uint64_t *results = (uint64_t *) malloc (num_threads * sizeof (uint64_t));

  for (int i = 0; i < num_threads; i++)
    {
      pthread_create (&threads[i], NULL, generator, (void *) primes[i]);
    }

  for (int i = 0; i < num_threads; i++)
    {
      pthread_join (threads[i], (void *) &results[i]);
    }

  return results;
}

double time_diff (struct timeval start, struct timeval end)
{
  double ending = end.tv_sec + (end.tv_usec * 0.000001);
  double starting = start.tv_sec + (start.tv_usec * 0.000001);
  return ending - starting;
}

/* Calculate the discrete logarithm for several values. That is, given the
   values g, p, and g^n mod p, determine n. (Note that this is an intentionally
   SLOW operation!) The parameters are stored in three global arrays:
     gens[] - this contains the generator values (g)
     primes[] - this contains the prime numbers (p)
     mod_powers[] - this contains the values g^n mod p for unknown n values
   In pseudocode, you should be doing:
     for i in start_index .. end_index-1
       logs [i] = discrete_log (parameters[i])
   The return value is the real time it takes to compute these values. Use
   the C standard library function gettimeofday() to get a start and end time,
   then use the helper function time_diff() to compute the difference.
 */
double
time_log (size_t start_index, size_t end_index, uint64_t * gens,
          uint64_t * primes, uint64_t * mod_powers, uint64_t * logs)
{
  struct timeval start_time;
  struct timeval end_time;
  gettimeofday (&start_time, NULL);

  for (int i = start_index; i < end_index; i++)
    {
      logs[i] = discrete_log (mod_powers[i], gens[i], primes[i]);
    }

  gettimeofday (&end_time, NULL);
  double time = time_diff (start_time, end_time);
  return time;
}

/* Wrapper function to call time_log() from within a thread */
void *
time_log_thread (void * _args)
{
  struct time_args *args = (struct time_args*) _args;

  double time_taken = time_log(args->start_index, args->start_index + args->number,
                                args->generators, args->primes, args->mod_powers, args->results);

  args->time_taken = time_taken;

  pthread_exit (NULL);
}
