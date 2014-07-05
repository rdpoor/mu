/*
 * ConstantSP is a source of constant values.  It simply outputs a
 * fixed value for every frame.
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

#ifndef MU_CONSTANT_STREAM_H
#define MU_CONSTANT_STREAM_H

#include "mu.h"
#include "tick_utils.h"
#include "sample_processor.h"

namespace mu {

  class ConstantSP : public SampleProcessor {
  public:

    ConstantSP( void );
    ~ConstantSP( void );

    std::string getClassName() { return "ConstantSP"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, bool is_new_event);

    stk::StkFloat getValue() const { return value_; }
    ConstantSP& setValue(stk::StkFloat value) { value_ = value; return *this; }
    
  protected:
    stk::StkFloat value_;
  };                            // class ConstantSP
}                               // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
