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

// File: crop_stream.h
// CropStream crops its input between a starting time (inclusive and ending time
// (exclusive).

#ifndef MU_CROP_STREAM_H
#define MU_CROP_STREAM_H

#include "single_source_stream.h"

namespace mu {
  class CropStream : public SingleSourceStream {
  public:

    CropStream() : source_start_(kUndefined), source_end_(kUndefined) {}
    ~CropStream() {}
    
    MuTick source_start() { return source_start_; }
    void set_source_start(MuTick source_start) { source_start_ = source_start; }

    MuTick source_end() { return source_end_; }
    void set_source_end(MuTick source_end) { source_end_ = source_end; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

  protected:
    MuTick source_start_;
    MuTick source_end_;
  };
    
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
