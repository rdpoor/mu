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
    
    static MuFloat default_f0() {
      return 440.0;
    }
    
    static MuFloat default_a0() {
      return 0.5;
    }
    
    static MuFloat default_p0() {
      return 0.0;
    }
    
    SineStream()
      : a0_(default_a0()),
        f0_(default_f0()),
        p0_(default_p0()),
        am_source_(NULL),
        pm_source_(NULL) {}
    
    double a0() { return a0_; }
    void set_a0(double a0) { a0_ = a0; }

    double f0() { return f0_; }
    void set_f0(double f0) { f0_ = f0; }

    double p0() { return p0_; }
    void set_p0(double p0) { p0_ = p0; }

    MuStream *am_source() { return am_source_; }
    void set_am_source(MuStream *am_source) {
      am_source_ = am_source;
    }

    MuStream *pm_source() { return pm_source_; }
    void set_pm_source(MuStream *pm_source) {
      pm_source_ = pm_source;
    }
    
    bool render(MuTick buffer_start, MuBuffer *buffer);

  protected:
    double a0_;                  // base amplitude, normally 1.0 max
    double f0_;                  // base frequencey in hz
    double p0_;                  // base phase in radians
    MuStream *am_source_;        // source of amplitude modulation
    MuStream *pm_source_;        // source of phase modulation
    MuBuffer am_buffer_;         // temp buffers for stream sources
    MuBuffer pm_buffer_;
  };                            // class SineStream
    
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
