/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  
   ================================================================ 
*/

// File: sine_stream.h
// SineStream represents a sine wave.

#ifndef MU_SINE_STREAM_H
#define MU_SINE_STREAM_H

#include "mu_stream.h"

namespace mu {
  class SineStream : public MuStream {
  public:
    
    static const MuFloat default_frequency() {
      return 440.0;
    }
    
    static const MuFloat default_amplitude() {
      return 0.5;
    }
    
    static const MuFloat default_phase() {
      return 0.0;
    }
    
    SineStream()
      : amplitude_(default_amplitude()),
        frequency_(default_frequency()),
        phase_(default_phase()) {}
    
    double amplitude() { return amplitude_; }
    void set_amplitude(double amplitude) { amplitude_ = amplitude; }

    double frequency() { return frequency_; }
    void set_frequency(double frequency) { frequency_ = frequency; }

    double phase() { return phase_; }
    void set_phase(double phase) { phase_ = phase; }

    bool render(MuBuffer &buffer, MuTick start_tick);

  protected:
    double amplitude_;          // normally 1.0 max
    double frequency_;          // hz
    double phase_;              // radians
  };
    
}

#endif

// Local Variables:
// mode: c++
// End:
