#include <assert.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pingpong.h"

typedef struct pparg
{
  sem_t *sem_ping; // Tell ping to wake up
  sem_t *sem_pong; // Tell pong to wake up
  int *shared;
} ppargs_t;

void *
pong (void *_args)
{
  ppargs_t *args = (ppargs_t *)_args;
  for (int i = 0; i < 10; i++)
    {
      // TODO: Wait on the pong semaphore. Once awakened, print "PONG!\n",
      // decrement shared integer, and awaken the ping thread, then wait
      // on the pong semaphore again.
      sem_wait (args->sem_pong);
      printf ("PONG!\n");
      (*args->shared)--;
      sem_post (args->sem_ping);
      // sem_wait (args->sem_pong);
    }
  printf ("PONG: Game over\n");
  pthread_exit (NULL);
}

int
ping (void)
{
  pthread_t pong_thread;
  sem_t *sem_ping;
  sem_t *sem_pong;
  int shared = 0;

  // TODO: Initialize the semaphores and set up the arguments for the pong()
  // thread so both point to the same semaphores and int.
  ppargs_t args;
  sem_ping = (sem_t *)malloc (sizeof (sem_t));
  sem_pong = (sem_t *)malloc (sizeof (sem_t));
  sem_init (sem_ping, 0, 1);
  sem_init (sem_pong, 0, 0);

  args.shared = &shared;
  args.sem_ping = sem_ping;
  args.sem_pong = sem_pong;

  pthread_create (&pong_thread, NULL, pong, (void *)&args);

  for (int i = 0; i < 10; i++)
    {
      // TODO: Wait on the ping semaphore. Once awakened, print "PING!\n",
      // increment shared integer, and awaken the pong thread, then wait
      // on the ping semaphore again.
      sem_wait (sem_ping);
      printf ("PING!\n");
      shared++;
      sem_post (sem_pong);
      // sem_wait (args->sem_ping);
    }

  pthread_join (pong_thread, NULL);

  printf ("PING: Game over\n");

  // TODO: Clean up the semaphores before returning.
  sem_destroy (sem_ping);
  sem_destroy (sem_pong);
  free (sem_ping);
  free (sem_pong);

  return shared;
}
