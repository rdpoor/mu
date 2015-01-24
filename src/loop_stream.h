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

// File: loop_stream.h
// LoopStream causes its input stream to loop every +interval+ ticks:
//
//   loop(source, t) = SUM[i=-inf to +inf] { source(t * i * interval) }
//
// The result may overlap with itself, so to prevent an infinite iteration, the
// start and end times of the source stream must be specified.
//

#ifndef MU_LOOP_STREAM_H
#define MU_LOOP_STREAM_H

#include "single_source_stream.h"
#include <limits.h>

namespace mu {
  class LoopStream : public SingleSourceStream {
  public:

    LoopStream() :
      interval_(44100),
      source_start_(0), 
      source_end_(44100) {}
      
    ~LoopStream() {}
    
    MuTick interval() { return interval_; }
    void set_interval(MuTick interval) { interval_ = interval; }

    MuTick source_start() { return source_start_; }
    void set_source_start(MuTick source_start) { source_start_ = source_start; }

    MuTick source_end() { return source_end_; }
    void set_source_end(MuTick source_end) { source_end_ = source_end; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

  protected:
    MuTick interval_;
    MuTick source_start_;
    MuTick source_end_;

  private:
    bool render_segment(MuBuffer *buffer, 
                        MuTick buffer_start, 
                        MuTick buffer_end,
                        MuTick delay);

  };                            // class LoopStream
    
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
