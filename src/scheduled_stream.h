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

// File: scheduled_stream.h
// Call scheduled events in the course of producing samples.

#ifndef MU_SCHEDULED_STREAM_H
#define MU_SCHEDULED_STREAM_H

#include "mu_scheduler.h"
#include "single_source_stream.h"

namespace mu {
  class ScheduledStream : public SingleSourceStream {
  public:
    
    ScheduledStream( void );
    virtual ~ScheduledStream( void );
    virtual ScheduledStream *clone( void );

    MuScheduler *scheduler() { return &scheduler_; }
    
    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "ScheduledStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);
    MuScheduler scheduler_;
  };                            // class ScheduledStream
    
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
