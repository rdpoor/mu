#include "assert.h"
#include <stdio.h>

// TODO: add optinal argument
// TODO: add controls for printing, stop on error, etc.
//
void assert(bool test, const char *msg, const char *file, const int line) {
  if (!test) {
    fprintf(stderr, "%s:%d:1: %s: fail\n", file, line, msg);
  } else {
    // fprintf(stderr, "%s:%d:1: %s: okay\n", file, line, msg);
  }
    
}
