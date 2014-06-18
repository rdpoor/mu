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
#include "loop_stream.h"

namespace mu {

  LoopStream::LoopStream()
    : loop_duration_ ( kDefaultLoopDuration ) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  LoopStream::~LoopStream() {
    TRACE("LoopStream::~LoopStream()\n");
  }

  void LoopStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getLoopDuration() = " << getLoopDuration() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void LoopStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    // fprintf(stderr,"LoopStream::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    if (source_ == NULL) { return; }

    long int frames_copied = 0;

    while (frames_copied < buffer.frames()) {
      // fprintf(stderr,"b");
      Tick current_tick = frames_copied + tick;
      // TODO: This truncates towards zero.  We really want floor()
      // instead to allow for negative tick times.  Make it a separate
      // function for testability.
      Tick prev_tick_point = (current_tick / loop_duration_) * loop_duration_;
      Tick next_tick_point = prev_tick_point + loop_duration_;
      Tick frames_to_copy = std::min((next_tick_point - current_tick), (buffer.frames() - frames_copied));
      // At this point:
      // current_tick is the global time of buffer[frames_copied]
      // frames_to_copy is how many frames we can copy before we run into a loop point
      //   or into the end of buffer
      // (current_tick % loop_duration_) is the time associated with buffer_[0]
      if (frames_to_copy == buffer.frames()) {
        // optimize the a common case: no copy needed
        // fprintf(stderr,"c");
        source_->step(buffer, (current_tick % loop_duration_), player);
        // fprintf(stderr,"c1");

      } else {
        // resize buffer_ to receive frames_to_copy frames from source
        // fprintf(stderr,"d");
        buffer_.resize(frames_to_copy, buffer.channels());
        // fill with source data
        // fprintf(stderr,"e");
        source_->step(buffer_, (current_tick % loop_duration_), player);
        // copy from buffer_[0] into buffer[frames_copied] for frames_to_copy frames
        // fprintf(stderr,"f");
        copyBuffer(buffer_, 0, buffer, frames_copied, frames_to_copy);

      }
      // fprintf(stderr,"g");
      frames_copied += frames_to_copy;
    }
  }

}
