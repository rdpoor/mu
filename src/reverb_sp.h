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

#ifndef MU_REVERB_STREAM
#define MU_REVERB_STREAM

#include "NRev.h"
#include "mu.h"
#include "single_source_sp.h"

namespace mu {

  class ReverbSP : public SingleSourceSP<ReverbSP> {
  public:

    ReverbSP( void );
    ~ReverbSP( void );

    std::string getClassName() { return "ReverbSP"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getEnd();

    stk::StkFloat getRatio( void ) { return wet_; }
    ReverbSP& setRatio(stk::StkFloat ratio) { 
      wet_ = ratio;
      dry_ = 1.0 - ratio;
      return *this;
    }
  
  protected:
    stk::StkFloat dry_;
    stk::StkFloat wet_;
    stk::NRev *reverb_;

  private:
    void setup() {
      if (reverb_ == NULL) {
        reverb_ = new stk::NRev();
        reverb_->setT60( 2.0 );
      }
      return;
    }

  }; // class ReverbSP
    
} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
