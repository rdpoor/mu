/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ================================================================
*/

#include "mu_utils.h"
#include "resample_stream.h"
#include <math.h>

namespace mu {

  ResampleStream::ResampleStream()
    : sample_source_ ( NULL ), 
      timing_source_ ( NULL ) {
    sample_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    timing_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ResampleStream::~ResampleStream() {
    // printf("~ResampleStream()\n");
    if (sample_source_ != NULL) delete sample_source_;
    if (timing_source_ != NULL) delete timing_source_;
  }

  ResampleStream *ResampleStream::clone() {
    ResampleStream *c = new ResampleStream();
    c->set_sample_source(sample_source() ? sample_source()->clone() : NULL);
    c->set_timing_source(timing_source() ? timing_source()->clone() : NULL);
    return c;
  }

  bool ResampleStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if ((sample_source_ == NULL) || (timing_source_ == NULL)) {
      return false;
    }

    timing_buffer_.resize(buffer->frames(), buffer->channels());
    MuUtils::zero_buffer(&timing_buffer_);
    if (!timing_source_->render(buffer_start, &timing_buffer_)) {
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
    sample_buffer_.resize(n_frames, buffer->channels());
    MuUtils::zero_buffer(&sample_buffer_);
    if (!sample_source_->render(min_tick, &sample_buffer_)) {
      return false;
    }

    MuTick buffer_end = buffer_start + buffer->frames();

    for (MuTick tick=buffer_start; tick<buffer_end; tick++) {
      // use channel 0 (only) for timing information
      MuFloat ftau = timing_buffer_(frame_index(buffer_start, tick), 0);
      MuTick itau = ( MuTick )ftau;
      MuFloat alpha = ftau - itau;
      for (int channel = buffer->channels()-1; channel >= 0; channel--) {
        if (alpha == 0.0) {
          (*buffer)(frame_index(buffer_start, tick), channel) = 
            sample_buffer_(frame_index(min_tick, itau), channel);
        } else {
          MuFloat y0 = sample_buffer_(frame_index(min_tick, itau), channel);
          MuFloat y1 = sample_buffer_(frame_index(min_tick, itau+1), channel);
          MuFloat dy = y1 - y0;
          (*buffer)(frame_index(buffer_start, tick), channel) = y0 + dy * alpha;
        }
      }
    }
    return true;
  }

  void ResampleStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss); 
    *ss << "sample_source()" << std::endl;
    sample_source()->inspect_aux(level+1, ss);
    inspect_indent(level, ss); 
    *ss << "timing_source()" << std::endl;
    timing_source()->inspect_aux(level+1, ss);
  }

}                               // namespace mu
