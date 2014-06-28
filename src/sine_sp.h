/*
 * SineSP is a source of sine values.
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

#ifndef MU_SINE_STREAM_H
#define MU_SINE_STREAM_H

#include "mu.h"
#include "sample_processor.h"

namespace mu {

  class SineSP : public SampleProcessor {
  public:

    // static const stk::StkFloat kDefaultAmplitude = 1.0;
    // static const stk::StkFloat kDefaultFrequency = 440.0;
    // static const stk::StkFloat kDefaultPhase = 0.0;
    
    SineSP( void );
    ~SineSP( void );

    std::string getClassName() { return "SineSP"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, Player& player);

    stk::StkFloat getAmplitude() const { return amplitude_; }
    SineSP& setAmplitude(stk::StkFloat amplitude) { amplitude_ = amplitude; return *this; }

    stk::StkFloat getFrequency() const { return frequency_; }
    SineSP& setFrequency(stk::StkFloat frequency) { frequency_ = frequency; return *this; }

    stk::StkFloat getPhase() const { return phase_; }
    SineSP& setPhase(stk::StkFloat phase) { phase_ = phase; return *this; }

    stk::StkFloat getPitch() const { return 69 + 12 * log2(frequency_/440.0); }
    SineSP& setPitch(stk::StkFloat pitch) { 
      frequency_ = pow(2, (pitch-69.0)/12.0) * 440.0; 
      return *this;
    }

  protected:
    stk::StkFloat amplitude_;
    stk::StkFloat frequency_;
    stk::StkFloat phase_;
  };                            // class SineSP
}                               // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
