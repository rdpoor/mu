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
#include "linseg_sp.h"

namespace mu {

  LinsegSP::LinsegSP( void ) 
    : hold_value_ ( kDefaultHoldValue ) {
  }

  LinsegSP::~LinsegSP( void ) {
  }

  // compute one value.  breakpoints_ is guaranteed to have at least
  // one breakpoint.
  // TODO: vectorize.
  stk::StkFloat LinsegSP::lerp(Tick tick) {
    if (tick < breakpoints_.begin()->first) {
      return hold_value_ ? breakpoints_.begin()->second : 0.0;

    } else if (tick == breakpoints_.begin()->first) {
      return breakpoints_.begin()->second;

    } else if (tick == breakpoints_.rbegin()->first) {
      return breakpoints_.rbegin()->second;

    } else if (tick > breakpoints_.rbegin()->first) {
      return hold_value_ ? breakpoints_.rbegin()->second : 0.0;

    } else {
      // tick is guaranteed to lie within two breakpoints
      Breakpoints::iterator cursor = breakpoints_.lower_bound(tick);
      Tick x1 = cursor->first;
      stk::StkFloat y1 = cursor->second;
      --cursor;
      Tick x0 = cursor->first;
      stk::StkFloat y0 = cursor->second;

      return y0 + (tick - x0)*(y1-y0)/(x1-x0);
    }

  }

  void LinsegSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (breakpoints_.size() == 0) {
      zeroBuffer(buffer);

    } else {
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      for (int frame=0; frame<frame_count; frame++) {
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = lerp(tick + frame);
        }
      }
    }
  }

  Tick LinsegSP::getStart() {
    if ((breakpoints_.size() == 0) || (hold_value_ == true)) {
      return kIndefinite;
    } else {
      return breakpoints_.begin()->first;
    }
  }

  Tick LinsegSP::getEnd() {
    if ((breakpoints_.size() == 0) || (hold_value_ == true)) {
      return mu::kIndefinite;
    } else {
      return breakpoints_.rbegin()->first;
    }
  }

  LinsegSP& LinsegSP::addBreakpoint(Tick tick, stk::StkFloat value) {
    breakpoints_[tick] = value;
    return *this;
  }

  LinsegSP& LinsegSP::removeAllBreakpoints() {
    breakpoints_.clear();
    return *this;
  }

}
