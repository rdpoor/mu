#include "delay_stream.h"

namespace mu {

  DelayStream::~DelayStream() {
    TRACE("DelayStream::~DelayStream()\n");
  }

  // If source has a frame count, its end time is extended by delay_
  // TODO: this is semantically odd.  We're not changing the duration
  // of the stream, just its start and stop times.  Perhaps we need a
  // streamExtent() method instead that returns start and end frames.
  Tick DelayStream::frameCount() {
    return (source_ == NULL) ? 
      kIndefinite : 
      ((source_->frameCount() == kIndefinite) ? kIndefinite : (source_->frameCount() + delay_));
  }

  DelayStream& DelayStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    if (source_ != NULL) {
      source_->step(buffer, tick - delay_, player);
    } else {
      bzero(&(buffer[0]), buffer.frames() * buffer.channels() * sizeof(stk::StkFloat));
    }
    return *this;
  }

}
