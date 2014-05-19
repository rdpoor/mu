#include "delay_stream.h"

namespace mu {

  DelayStream::DelayStream() 
    : delay_ ( 0 ) {
    TRACE("DelayStream::DelayStream()\n");
  }

  DelayStream::~DelayStream() {
    TRACE("DelayStream::~DelayStream()\n");
  }

  void DelayStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getDelay() = " << getDelay() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  DelayStream& DelayStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      source_->step(buffer, tick - delay_, player);
    }
    return *this;
  }

  Tick DelayStream::getStart() {
    if ((source_ == NULL) || (source_->getStart() == kIndefinite)) {
      return kIndefinite;
    } else {
      return (source_->getStart() + delay_);
    }
  }

  Tick DelayStream::getEnd() {
    if ((source_ == NULL) || (source_->getEnd() == kIndefinite)) {
      return kIndefinite;
    } else {
      return (source_->getEnd() + delay_);
    }
  }

}
