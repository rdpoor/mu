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
#include "resample_rs.h"
#include <math.h>

namespace mu {

  ResampleRS::ResampleRS()
    : sample_source_ ( NULL ), 
      timing_source_ ( NULL ) {
    sample_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    timing_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ResampleRS::~ResampleRS() {
  }

  bool ResampleRS::render(stk::StkFrames &buffer, MuTick base_tick, MuTick start_tick, MuTick end_tick) {

    zero_buffer(buffer);
    if ((sample_source_ == NULL) || (timing_source_ == NULL)) {
      // printf("=== sample_source_ or timing_source_ is null\n");
      return false;
    }

    timing_buffer_.resize(buffer.frames(), buffer.channels());
    if (!timing_source_->render(timing_buffer_, base_tick, start_tick, end_tick)) {
      // printf("=== timing_source_.render() returned false\n");
      return false;
    }

    MuFloat min = (double)LONG_MAX;
    MuFloat max = (double)LONG_MIN;

    // find min and max times in timing_buffer_
    for (int i=timing_buffer_.frames()-1; i>=0; i--) {
      MuFloat v = timing_buffer_(i, 0); // channel 0 only
      min = (v < min) ? v : min;
      max = (v > max) ? v : max;
    }
    
    MuTick min_tick = (MuTick)floor(min);
    MuTick max_tick = (MuTick)ceil(max);
    // add 1 to for sample[i+1] interpolation.
    MuTick n_frames = (max_tick - min_tick) + 1;

    // Fetch source samples
    sample_buffer_.resize(n_frames, buffer.channels());
    if (!sample_source_->render(sample_buffer_, min_tick, min_tick, min_tick + sample_buffer_.frames())) {
      // printf("=== sample_source_.render(%ld, %ld, %ld) returned false\n", min_tick, min_tick, min_tick + sample_buffer_.frames());
      return false;
    }

    for (MuTick tick=start_tick; tick<end_tick; tick++) {
      MuFloat ftau = timing_buffer_(frame_index(base_tick, tick), 0); // channel 0 only
      MuTick itau = ( MuTick )ftau;
      MuFloat alpha = ftau - itau;
      for (int channel = buffer.channels()-1; channel >= 0; channel--) {
        if (alpha == 0.0) {
          buffer(frame_index(base_tick, tick), channel) = 
            sample_buffer_(frame_index(min_tick, itau), channel);
        } else {
          MuFloat y0 = sample_buffer_(frame_index(min_tick, itau), channel);
          MuFloat y1 = sample_buffer_(frame_index(min_tick, itau+1), channel);
          MuFloat dy = y1 - y0;
          buffer(frame_index(base_tick, tick), channel) = y0 + dy * alpha;
        }
      }
    }
    return true;
  }

}
