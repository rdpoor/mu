#include "assert.h"
#include <stdio.h>

// TODO: add optinal argument
// TODO: add controls for printing, stop on error, etc.
//
void assert(bool test, const char *msg) {
  if (!test) {
    fprintf(stderr, "%s failed\n", msg);
  }
}
