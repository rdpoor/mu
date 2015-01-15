/*
  StkEffectRS: simplify using an STK instrument as an effect.

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

#ifndef MU_STK_EFFECT_RS
#define MU_STK_EFFECT_RS

#include "single_source_rs.h"

namespace mu {

  class StkEffectRS : public SingleSourceRS {
  public:

    StkEffectRS() : blend_(1.0) {
      source_ = NULL;
    }
    ~StkEffectRS() {}

    // blend of 1.0 = 100% effect, 0.0 = 0% effect
    MuFloat blend( void ) { return blend_; }
    void set_blend(MuFloat blend) { blend_ = blend; }
  
    bool render(stk::StkFrames& buffer, MuTick base_tick, MuTick start_tick, MuTick end_tick);

    // Pass one sample into the StkEffect.  In the current
    // implementation, this is the sum of all channels.
    virtual void tick(MuFloat value) = 0;

    // Fetch the result of the last call to tick() for the specified
    // channel.
    virtual MuFloat last_out(int channel) = 0;

  protected:
    MuFloat blend_;

  }; // class StkEffectRS
    
} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
