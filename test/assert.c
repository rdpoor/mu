#include "assert.h"
#include <stdio.h>

#define EPSILON 0.0001

bool near(double f1, double f2) {
  double d = EPSILON;
  bool pass = ((f1 - d) < f2) & ((f1 + d) > f2);
  if (pass == false) {
    fprintf(stderr,"near: %f should be near %f\n", f1, f2);
  }
  return pass;
}

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
