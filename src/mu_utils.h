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

// File: mu_utils.h
// General utilities in support of Mu data structures and types.

#ifndef MU_UTILS_H
#define MU_UTILS_H

#include "mu_types.h"

namespace mu {

  class MuUtils {
  public:

    // Fill buffer with a constant value
    static void fill_buffer(MuBuffer &buffer, MuFloat value) {
      int n_channels = buffer.channels();
      for (MuTick tick = buffer.frames()-1; tick >= 0; tick--) {
        for (int channel = n_channels-1; channel >= 0; channel--) {
          buffer(tick, channel) = value;
        }
      }
    }
    
    // Set contents of buffer to all zeroes.
    static void zero_buffer(MuBuffer &buffer) {
      fill_buffer(buffer, 0.0);
    }

    // Copy a subset of the contents of src into dst: src[i] is copied into
    // dst[i+offset] for count frames.  Assumes that the channel counts match.
    static void copy_buffer_subset(MuBuffer &src, 
                                   MuBuffer &dst, 
                                   MuTick offset, 
                                   MuTick count) {
      int n_channels = src.channels();
      for (int src_frame = count-1; src_frame >= 0; src_frame--) {
        for (int channel = n_channels-1; channel >= 0; channel--) {
          dst(src_frame + offset, channel) = src(src_frame, channel);
        }
      }
    }

    // Copy the contents of src into dst.  Assumes that dst is same size and
    // channel count as src.
    static void copy_buffer(MuBuffer &src, MuBuffer &dst) {
      copy_buffer_subset(src, dst, 0, src.frames());
    }

  };

}

#endif

// Local Variables:
// mode: c++
// End:
