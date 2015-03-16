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

#ifndef MU_TICK_UTILS_H
#define MU_TICK_UTILS_H

#include "mu.h"
#include <limits.h>

namespace mu {

  typedef long int Tick;

  class TickUtils {
  public:
    static Tick indefinite() { return LONG_MAX; }

    static bool isIndefinite(Tick t) { return indefinite() == t; }

    static Tick sum(Tick t0, Tick t1) {
      return (isIndefinite(t0) || isIndefinite(t1)) ? indefinite() : (t0 + t1);
    }

    static Tick difference(Tick t0, Tick t1) {
      return (isIndefinite(t0) || isIndefinite(t1)) ? indefinite() : (t0 - t1);
    }

    static Tick min(Tick t0, Tick t1) {
      return (isIndefinite(t0) || isIndefinite(t1)) ? indefinite() : std::min(t0, t1);
    }

    static Tick max(Tick t0, Tick t1) {
      return (isIndefinite(t0) || isIndefinite(t1)) ? indefinite() : std::max(t0, t1);
    }

    // Need:
    // does [s0,e0] overlap with [s1,e1]
    // how much of [s0,e0] overlaps with [s1,e1]

  };                            // class TickUtils

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
