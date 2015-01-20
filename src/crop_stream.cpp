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

#include "crop_stream.h"

namespace mu {

  bool CropStream::render(MuBuffer &buffer, MuTick buffer_start) {
    if (source_ == NULL) { return false; }

    MuTick buffer_end = buffer.frames() + buffer_start;
    MuTick earliest = buffer_start;
    if (start_time_ != kNotSet) {
      earliest = max(buffer_start, start_time_);
    }
    MuTick latest = buffer_end;
    if (end_time_ != kNotSet) {
      latest = min(buffer_end, end_time);
    }

    if ((earliest >= buffer_end) || (latest < buffer_start)) {
      // nothing to write
      return false;

    } else if ((earliest == buffer_start) && (latest == buffer_end)) {
      // common case: write the whole buffer
      return source_->render(buffer, buffer_start);

    } else if (earliest == latest) {
      return false;

    } else {
      // get samples between earliest and latest
      tmp_buffer_.resize(latest-earliest, buffer.channels());
      bool any_written = source_->render(&tmp_buffer_, earliest);
      if (any_written) {
        zero_buffer(buffer);
        copy_buffer_subset(tmp_buffer_, 
                           buffer_, 
                           earliest-start_frame,
                           latest-earliest);
      }
      return any_written;
    }

  }

}
