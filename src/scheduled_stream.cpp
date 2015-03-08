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

#include "scheduled_stream.h"
#include "mu_utils.h"

namespace mu {

  ScheduledStream::ScheduledStream( void ) {
  }

  ScheduledStream::~ScheduledStream( void ) {
    // printf("~ScheduledStream()\n");
  }

  ScheduledStream *ScheduledStream::clone( void ) {
    ScheduledStream *c = new ScheduledStream();
    // TODO: Should be defined in SingleSourceStream
    c->set_source(source() ? source()->clone() : NULL);
    // TODO: copy contents of scheduler queue
    return c;
  }

  bool ScheduledStream::render(MuTick buffer_start, MuBuffer *buffer) {
    MuTick buffer_end = buffer_start + buffer->frames();
    int n_channels = buffer->channels();

    if (source_ == NULL) {
      // no source attached
      return false;

    } else if ((scheduler_.event_count() == 0) || (scheduler_.next_time() >= buffer_end)) {
      // no pending event that will affect this buffer
      return source_->render(buffer_start, buffer);

    } else {
      MuTick t0 = buffer_start;
      bool any_rendered = false;

      while ((scheduler_.event_count() > 0) && (scheduler_.next_time() < buffer_end)) {
	MuTick t1 = scheduler_.next_time();

	// render frames prior to the next event (if any)
	int n_frames = t1 - t0;
	if (n_frames > 0) {
	  tmp_buffer_.resize(n_frames, n_channels);
	  MuUtils::zero_buffer(&tmp_buffer_);
	  if (source_->render(t0, &tmp_buffer_)) any_rendered = true;
	}	  

	// perform the event
	scheduler_.step();

	// advance t0
	if (n_frames > 0) {
	  t0 = t1;
	}
      }

      // render any remaining frames between t0 and end of buffer
      if (t0 < buffer_end) {
	tmp_buffer_.resize(buffer_end - t0, n_channels);
	MuUtils::zero_buffer(&tmp_buffer_);
	if (source_->render(t0, &tmp_buffer_)) any_rendered = true;
      }

      return any_rendered;
    }
  }

  void ScheduledStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss); 
    SingleSourceStream::inspect_aux(level, ss);
  }
  

}                               // namespace mu

