#include "loop_stream.h"

namespace mu {

  LoopStream::~LoopStream() {
    TRACE("LoopStream::~LoopStream()\n");
  }
  
  LoopStream& LoopStream::step(stk::StkFrames& buffer, 
                       Tick tick,
                       Player& player) {
    long int frames_copied = 0;
    while (frames_copied < buffer.frames()) {
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
        source_->step(buffer, (current_tick % loop_duration_), player);
      } else {
        // resize buffer_ to receive frames_to_copy frames from source
        buffer_.resize(frames_to_copy, buffer.channels());
        // fill with source data
        source_->step(buffer_, (current_tick % loop_duration_), player);
        // copy from buffer_[0] into buffer[frames_copied] for frames_to_copy frames
#if 1
        copy_buffer(buffer_, 0, buffer, frames_copied, frames_to_copy);
#else
        int samples_to_copy = frames_to_copy * buffer.channels();
        int dst = frames_copied * buffer.channels();
        for (int src=0; src<samples_to_copy; src++, dst++) { buffer[dst] = buffer_[src]; }
#endif
      }
      frames_copied += frames_to_copy;
    }

    return *this;
  }

}
