/*
 * CS 361: Template project driver
 *
 * Name: Grayson Diehl
 */

#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "discrete.h"
#include "pthread.h"

int cmdline (int, char**, size_t*, uint64_t**, uint64_t**, uint64_t**, size_t*);

void
usage (void)
{
  printf ("Usage: threads [options] (g:p:g2n ... )\n");
  printf (" Required arguments must be an array of g:p:g2n values\n");
  printf (" Options are:\n");
  printf ("  -n N    Create N concurrent threads\n");
  printf (" g:p:g2n is a colon-delimited list of 3 uint64_t values\n");
  printf ("  g       A group generator\n");
  printf ("  p       A prime number\n");
  printf ("  g2n     g^n mod p for some unknown n\n");
}

int
main (int argc, char **argv)
{
  uint64_t *primes = NULL;
  uint64_t *gens = NULL;
  uint64_t *powers = NULL;
  size_t num_threads = 1;
  size_t num_args = 0;

  if (cmdline (argc, argv, &num_args, &primes, &gens, &powers, &num_threads) < 0)
    {
      usage ();
      return EXIT_FAILURE;
    }

  if (num_args < 1 || num_args > 20)
    {
      printf ("There must be between 1 and 20 arguments\n");
      return EXIT_FAILURE;
    }

  // Unithreaded version to serve as a template
  if (num_threads == 1)
    {
      for (int i = 0; i < num_args; i++)
        {
          uint64_t result = discrete_log (powers[i], gens[i], primes[i]);
          printf ("Given g = %"PRId64", p = %"PRId64", g2n = %"PRId64" => n = %"PRId64"\n",
                  gens[i], primes[i], powers[i], result);
        }

      // return EXIT_SUCCESS;
    }
  else
    {
      // TODO: Compute the same values in parallel
      // Output should be identical to the unithreaded version from above
      // Print format for each prime entered:
      // printf ("Given g = %"PRId64", p = %"PRId64", g2n = %"PRId64" => n = %"PRId64"\n",
      //         generator, prime, power, n);

      pthread_t threads[num_threads];
      struct time_args *_args[num_threads];

      unsigned long start = 0;
      unsigned long number_per_thread = num_args / num_threads;
      uint64_t *result = malloc(num_args * sizeof(uint64_t));

      for (int i = 0; i < num_threads; i++)
      {
        _args[i] = malloc (sizeof (struct time_args));
        _args[i]->start_index = start;
        _args[i]->number = number_per_thread;
        _args[i]->generators = gens;
        _args[i]->primes = primes;
        _args[i]->mod_powers = powers;
        _args[i]->results = result;

        start += number_per_thread;

        pthread_create(&threads[i], NULL, time_log_thread, _args[i]);
      }

      for (int i = 0; i < num_threads; i++)
      {
        pthread_join(threads[i], NULL);
      }

      for (int i = 0; i < num_args; i++)
      {
        printf("Given g = %" PRId64 ", p = %" PRId64 ", g2n = %" PRId64 " => n = %" PRId64 "\n",
              gens[i], primes[i], powers[i], result[i]);
      }

      for (int i = 0; i < num_threads; i++)
      {
        free(_args[i]);
      }
      free(result);
    }

  if (primes != NULL)
    free (primes);
  if (gens != NULL)
    free (gens);
  if (powers != NULL)
    free (powers);

  return EXIT_SUCCESS;
}

/*****************************************************************************
 ****************** DO NOT MODIFY FUNCTIONS IN THIS SECTION ******************
 *****************************************************************************/

int
cmdline (int argc, char **argv, size_t *length, uint64_t **primes,
         uint64_t **gens, uint64_t **powers, size_t *num_threads)
{
  int option;

  while ((option = getopt (argc, argv, "n:h")) != -1)
    {
      switch (option)
        {
        case 'n': *num_threads = (size_t) strtol (optarg, NULL, 10);
                  break;
        case 'h': return -1;
                  break;
        default:  return -1;
        }
    }

  if (optind == argc)  return -1;
  *length = (size_t) (argc - optind);
  *primes = calloc (*length, sizeof (uint64_t));

  size_t index;
  // These must both have g:p:g2n values
      *gens = calloc (*length, sizeof (uint64_t));
      *powers = calloc (*length, sizeof (uint64_t));
      for (index = optind; index < argc; index++)
        {
          char *token = strtok (argv[index], ":");
          uint64_t value = strtol (token, NULL, 10);
          (*gens)[index - optind] = value;
          token = strtok (NULL, ":");
          assert (token != NULL);
          value = strtol (token, NULL, 10);
          (*primes)[index - optind] = value;
          token = strtok (NULL, ":");
          assert (token != NULL);
          value = strtol (token, NULL, 10);
          (*powers)[index - optind] = value;
        }

  return 0;
}
