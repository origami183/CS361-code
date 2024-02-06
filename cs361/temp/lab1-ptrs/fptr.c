#include "fptr.h"

// Fix this so that run_func takes three parameters: two ints and a
// single function pointer. The function pointed to by the pointer should
// take two ints and return an int. The body of the function should call
// the passed function with the two provided values, returning the result.
int
run_func (int x, int y, int (*f) (int a, int b))
{
  int result = f (x, y);
  return result;
}
