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

#include "sine_stream.h"
#include <math.h>

namespace mu {

  bool SineStream::render(MuTick buffer_start, MuBuffer *buffer) {
    int n_frames = buffer->frames();
    int n_channels = buffer->channels();
    double data_rate = buffer->dataRate();

    // OPTIMIZATION: split this into eight functions each with its own
    // particular inner loop.

    if (am_source_) {
      am_buffer_.resize(n_frames, n_channels);
      am_source_->render(buffer_start, &am_buffer_);
    }
    if (pm_source_) {
      pm_buffer_.resize(n_frames, n_channels);
      pm_source_->render(buffer_start, &pm_buffer_);
    }

    // OPTIMIZATION: special case 1 and 2 channels

    double omega = f0_ * 2.0 * M_PI;

    for (MuTick tick=n_frames-1; tick >= 0; tick--) {
      for (int channel = n_channels-1; channel >= 0; channel--) {
        double a = a0_;
        if (am_source_) a += am_buffer_(tick, channel);
        double p = p0_;
        if (pm_source_) p += pm_buffer_(tick, channel);
        double t = (tick + buffer_start) / data_rate;
        double v = a * sin(omega * t + p);
        (*buffer)(tick, channel) = v;
      }
    }

    return true;
  }

  void SineStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "a0() = " << a0() << std::endl;
    inspect_indent(level, ss);
    *ss << "f0() = " << f0() << std::endl;
    inspect_indent(level, ss);
    *ss << "p0() = " << p0() << std::endl;
    inspect_indent(level, ss); 
    *ss << "am_source()" << std::endl;
    am_source()->inspect_aux(level+1, ss);
    *ss << "pm_source()" << std::endl;
    pm_source()->inspect_aux(level+1, ss);
  }


}                               // namespace mu
