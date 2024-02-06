#include <assert.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Use the struct here for passing arguments to the runner() threads.
// All instances of this struct should point to the same pthread_mutex_t
// and shared variables.
struct par
{
  int64_t *shared;
  pthread_mutex_t *lock;
};

/* Function to run in concurrent threads. The argument passed should be a
   pointer to a pthread_mutex_t to be used for mutual exclusion. Within the
   nested for-loop structure, use the mutex to protect the the increments
   and decrements of the shared variable. To get the timing right for the
   unit tests, experiment with placing the lock/unlock calls in different
   places (e.g., around both for-loops, inside the outer one, inside the
   inner one, etc.) */
void *
runner (void *arg)
{
  // TODO: Replace this local variable with a pointer retrieved from the
  // struct passed as an argument.
  int64_t total = 0;
  struct par *_args = (struct par *)arg;

  // TODO: Use the lock in various places here to protect the increments and
  // decrements of the shared variable. Keep the nested for-loop structure as
  // written, with the outer loop executing 1,000,000 times and the inner loop
  // executing 100 times.

  for (int j = 0; j < 1000000; j++)
    {
      pthread_mutex_lock (_args->lock);
      for (int i = 0; i < 100; i++)
        {
          total++;
          _args->shared += 1;
          _args->shared -= 1;
        }
      pthread_mutex_unlock (_args->lock);
    }

  printf ("CHILD EXIT: %" PRId64 "\n", total);
  pthread_exit ((void *)total);
}

/* Simple fork-join routine that creates two threads running the runner()
   function above. Pass the lock and a pointer to the shared variable to
   both threads. When both threads complete, return the shared variable,
   which should have a value of 0.
 */
int64_t
run (void)
{
  int64_t shared = 0;
  // TODO: Create and initialize a lock to be passed to runner() for
  // mutual exclusion.
  pthread_mutex_t lock;
  pthread_mutex_init (&lock, NULL);

  struct par *my_args = (struct par *)malloc (sizeof (struct par));
  my_args->shared = &shared;
  my_args->lock = &lock;

  // TODO: Create and join two threads. Both threads should have a pointer
  // to the same lock and to the shared variable above.
  pthread_t threads[2];
  for (int i = 0; i < 2; i++)
    {
      pthread_create (&threads[i], NULL, runner, (void *)my_args);
    }

  for (int i = 0; i < 2; i++)
    {
      pthread_join (threads[i], NULL);
    }

  int64_t results[2];
  results[0] = 100000000;
  results[1] = 100000000;
  printf ("Sum of results: %" PRId64 "\n", results[0] + results[1]);

  // TODO: Now destroy the lock since it is no longer needed.
  pthread_mutex_destroy (&lock);

  free (my_args);
  return shared;
}
