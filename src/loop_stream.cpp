#include "loop_stream.h"

namespace mu {

  LoopStream::LoopStream()
    : loop_duration_ (44100) {
    TRACE("LoopStream::LoopStream()\n");
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

  LoopStream& LoopStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    // fprintf(stderr,"LoopStream::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    if (source_ == NULL) { return *this; }

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

    return *this;
  }

}
