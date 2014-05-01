#ifndef MU_MU_H
#define MU_MU_H

#include <stdio.h>
#include "Stk.h"

namespace mu {

#define TRACE(s)
  // #define TRACE(s) fprintf(stderr, (s))

  typedef long int Tick;
  static const Tick kIndefinite = LONG_MAX;

  typedef double MuTime;

  // resolve a circular dependency
  class Stream;
  class Player;

  // General purpose clamp: clamp val to lie within min...max (inclusive)
  template <class T> const T& clamp(const T& min, const T& val, const T& max) {
    return (val < min) ? min : ((val > max) ? max : val);
  }

};

#endif
