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

// File: gain_stream.h
// GainStream applies a gain to a signal.  If there is a gain_source specified,
// then the signal is multiplied times that.  Otherwise, gain is determined by
// the fixed gain parameter.

#ifndef MU_GAIN_STREAM_H
#define MU_GAIN_STREAM_H

#include "mu_stream.h"

namespace mu {
  class GainStream : public MuStream {
  public:
    
    static MuFloat default_gain() {
      return 1.0;
    }
    
    GainStream( void );
    virtual ~GainStream( void );
    virtual GainStream *clone( void );

    double gain() { return gain_; }
    void set_gain(double gain) { gain_ = gain; }

    MuStream *signal_source() { return signal_source_; }
    void set_signal_source(MuStream *signal_source) {
      signal_source_ = signal_source;
    }

    MuStream *gain_source() { return gain_source_; }
    void set_gain_source(MuStream *gain_source) {
      gain_source_ = gain_source;
    }

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "GainStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    double gain_;                // base gain, normally 1.0 max
    MuStream *signal_source_;    // source of signal
    MuStream *gain_source_;      // source of gain
    MuBuffer gain_buffer_;       // temp buffer
  };                             // class GainStream
    
} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
