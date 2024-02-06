#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "discrete.h"

/* Find a generator for Z_prime* */
uint64_t
find_generator (uint64_t prime)
{
  uint64_t g = prime/2;   // candidate generator
  uint64_t n;             // the nth power of g
  uint64_t product = g;   // product = g^n mod p
  bool searching = true;  // still searching for g

  while (searching)
    {
      product = g;
      searching = false;  // optimistically guess g is good
      // stop at prime - 2, because g^(prime-1) = 1 for Z_p
      for (n = 1; n < prime - 2; n++)
        {
          product = (product * g) % prime;
          if (product == 1)
            {
              // FAIL: g is not a generator
              g++;
              if (g == prime) return 0; // should never get here
              searching = true;
              n = prime - 2;
            }
        }
    }

  return g;
}

/* Given g^n mod p, g, and p, compute n */
uint64_t
discrete_log (uint64_t g2n, uint64_t g, uint64_t p)
{
  uint64_t value = g;
  uint64_t n = 1;
  while (value != g2n)
    {
      value = (value * g) % p;
      n++;
    }

  return n;
}

