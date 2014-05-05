#ifndef MU_MU_H
#define MU_MU_H

#include <stdio.h>
#include "Stk.h"
#include "tick_indefinite.h"

namespace mu {

#define TRACE(s)
  // #define TRACE(s) fprintf(stderr, (s))

  typedef long int Tick;
  static const Tick kIndefinite = LONG_MAX;
  // Perhaps someday Tick will be a full-fledged class that allows an
  // indefinite value.  For now, that would require a lot of retrofits...
  // static const TickIndefinite kIndefinite = TickIndefinite::kIndefinite;

  typedef double MuTime;

  // resolve circular dependencies
  class Stream;
  class Player;

  // Type definition for a vector of Stream pointers
  typedef std::vector<mu::Stream *> StreamVector;

  // General purpose clamp: clamp val to lie within min...max (inclusive)
  template <class T> const T& clamp(const T& min, const T& val, const T& max) {
    return (val < min) ? min : ((val > max) ? max : val);
  }

};

#endif
