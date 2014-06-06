/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/

#ifndef MU_MU_H
#define MU_MU_H

#include <stdio.h>
#include "Stk.h"
#include "tick_indefinite.h"
#include <math.h>

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

  double dbToValue(double db);

};

#endif

// Local Variables:
// mode: c++
// End:
