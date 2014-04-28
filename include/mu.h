#ifndef MU_MU_H
#define MU_MU_H

#include <stdio.h>
#include "Stk.h"

namespace mu {

  // #define TRACE(s)
#define TRACE(s) fprintf(stderr, (s))

  typedef double MuTime;

  // resolve a circular dependency
  class Node;
  class Transport;

};

#endif
