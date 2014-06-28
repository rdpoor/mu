/*
 * LinsegSP generates linearly interpolated samples beween given
 * [tic,y] breakpoints.
 */
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

#ifndef MU_LINSEG_STREAM
#define MU_LINSEG_STREAM

#include "mu.h"
#include "sample_processor.h"
#include <map>

namespace mu {

  typedef std::map<mu::Tick, stk::StkFloat> Breakpoints;

  class LinsegSP : public SampleProcessor {
  public:

    static const bool kDefaultHoldValue = true;

    LinsegSP( void );
    ~LinsegSP( void );

    std::string getClassName() { return "LinsegSP"; }

    void step(stk::StkFrames& buffer, Tick tick, Player &player);
    
    Tick getStart( void );
    Tick getEnd( void );

    LinsegSP& addBreakpoint(Tick tick, stk::StkFloat value);
    LinsegSP& removeAllBreakpoints();
    size_t getBreakpointCount() { return breakpoints_.size(); }

    bool getHoldValue() { return hold_value_; }
    LinsegSP& setHoldValue(bool hold_value) { hold_value_ = hold_value; return *this; }

  private:
    stk::StkFloat lerp(Tick tick);

  protected:
    bool hold_value_;
    std::map<Tick, stk::StkFloat> breakpoints_;
  };

}

#endif


// Local Variables:
// mode: c++
// End:
