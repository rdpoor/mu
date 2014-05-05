#include "delay_stream.h"

namespace mu {

  DelayStream::~DelayStream() {
    TRACE("DelayStream::~DelayStream()\n");
  }

  DelayStream& DelayStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    if (source_ == NULL) {
      zero_buffer(buffer);
    } else {
      source_->step(buffer, tick - delay_, player);
    }
    return *this;
  }

  Tick DelayStream::getStart() {
    return (source_ == NULL) ? 
      kIndefinite : 
      ((source_->getStart() == kIndefinite) ? kIndefinite : (source_->getStart() + delay_));
  }

  Tick DelayStream::getEnd() {
    return (source_ == NULL) ? 
      kIndefinite : 
      ((source_->getEnd() == kIndefinite) ? kIndefinite : (source_->getEnd() + delay_));
  }


}
