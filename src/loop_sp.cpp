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
#include "loop_sp.h"

namespace mu {

  LoopSP::LoopSP() :
    loop_duration_ ( kDefaultLoopDuration ),
    start_ ( TickUtils::indefinite() ),
    end_ ( TickUtils::indefinite() ) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  LoopSP::~LoopSP() {
    TRACE("LoopSP::~LoopSP()\n");
  }

  void LoopSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getLoopDuration() = " << getLoopDuration() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void LoopSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    // fprintf(stderr,"LoopSP::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    if (source_ == NULL) { 
      zeroBuffer(buffer);
      return; 
    }

    Tick start_tick = tick;
    Tick end_tick = tick + buffer.frames();
    if (!TickUtils::isIndefinite(start_)) start_tick = std::max(start_tick, start_);
    if (!TickUtils::isIndefinite(end_)) end_tick = std::min(end_tick, end_);
    Tick n_frames = end_tick - start_tick;

    if (n_frames < buffer.frames()) zeroBuffer(buffer);

    long int frames_copied = 0;

    while (frames_copied < n_frames) {
      Tick current_tick = frames_copied + start_tick;
      Tick prev_tick_point = floor((double)current_tick / loop_duration_) * loop_duration_;
      Tick next_tick_point = prev_tick_point + loop_duration_;
      Tick frames_to_copy = std::min((next_tick_point - current_tick), (n_frames - frames_copied));
      // At this point:
      // current_tick is the global time of buffer[frames_copied]
      // frames_to_copy is how many frames we can copy before we run into a loop point
      //   or into the end of buffer
      // (current_tick % loop_duration_) is the time associated with buffer_[0]
      bool is_new = is_new_event || (current_tick % loop_duration_) == 0;
      if (frames_to_copy == buffer.frames()) {
        // optimize a common case: no copy needed
        source_->step(buffer, (current_tick % loop_duration_), is_new);
        // fprintf(stderr,"c1");

      } else {
        // resize buffer_ to receive frames_to_copy frames from source
        buffer_.resize(frames_to_copy, buffer.channels());
        // fill with source data
        source_->step(buffer_, (current_tick % loop_duration_), is_new);
        // copy from buffer_[0] into buffer[frames_copied] for frames_to_copy frames
        copyBuffer(buffer_, 0, buffer, frames_copied + (start_tick - tick), frames_to_copy);

      }
      frames_copied += frames_to_copy;
    }
  }

}
