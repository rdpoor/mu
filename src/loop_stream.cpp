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

#include "loop_stream.h"
#include "mu_utils.h"
#include <cmath>

namespace mu {

  bool LoopStream::render(MuBuffer &buffer, MuTick buffer_start) {
    if (source_ == NULL) { return false; }

    MuTick buffer_end = buffer_start + buffer.frames();

    // Compute how many segments we need to sum into buffer and then render
    // them.  We compute segments from k0 (inclusive) to segments k1
    // (exclusive), where k0 represents the first segment where the source's
    // end_time exceeds the buffer's start time and k1 is the first segment
    // where source's start_time equals or exceeds the buffer's end time.

    // k0 is the smallest integer such that 
    //   source_end_ + interval_ * k0 > buffer_start
    //   k0 > (buffer_start - source_end_) / interval_
    int k0 = floor((double)(buffer_start - source_end_)/(double)interval_) + 1;

    // k1 is the smallest integer such that
    //   source_start_ + interval_ * k1 >= buffer_end
    //   k1 >= (buffer_end - source_start_) / interval_
    int k1 = ceil((double)(buffer_end - source_start_)/(double)interval_);

    if (k0 >= k1) { return false; }

    bool any_written = false;
    MuUtils::zero_buffer(buffer);
    for (int k=k0; k<k1; k++) {
      MuTick delay = k * interval_;
      if (render_segment(buffer, buffer_start, buffer_end, delay)) {
        any_written = true;
      }
    }
    
    return any_written;
  }

  // Add source stream delayed by `delay` frames into buffer.  Returns
  // true if any frames were availble from the source.
  bool LoopStream::render_segment(MuBuffer &buffer, 
                                  MuTick buffer_start, 
                                  MuTick buffer_end,
                                  MuTick delay) {
    // Add source[N - delay] into buffer[N], but:
    // source extent is limited to source_start_ to source_end_
    // buffer extent is limited to buffer_start to buffer_end.
    // Calculate the extent of N:
    MuTick s1 = std::max(source_start_ + delay, buffer_start);
    MuTick e1 = std::min(source_end_ + delay, buffer_end);
    if (s1 >= e1) { return false; }

    // printf("processing %3ld samples:\n", e1-s1);
    // printf("  src start = %3ld\n", s1 - delay);
    // printf("  dst start = %3ld\n", s1);

    tmp_buffer_.resize(e1 - s1, buffer.channels());
    if (source_->render(tmp_buffer_, s1 - delay) == false) { return false; }

    // tmp_buffer_ now contains samples from the source to be added into buffer.
    // tmp_buffer[0] contains the sample for (output) time s1.
    for (MuTick frame=s1; frame<e1; frame++) {
      for (int channel=buffer.channels()-1; channel >= 0; channel--) {
        buffer(frame-buffer_start, channel) += tmp_buffer_(frame - s1, channel);
      }
    }

    return true;
  }

}
