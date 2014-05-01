#include "mix_stream.h"

namespace mu {

  MixStream::~MixStream() {
    TRACE("MixStream::~MixStream()\n");
  }

  MixStream& MixStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    if ((source_a_ == NULL) && (source_b_ == NULL)) {
      zero_buffer(buffer);
    } else if (source_b_ == NULL) {
      source_a_->step(buffer, tick, player);
    } else if (source_a_ == NULL) {
      source_b_->step(buffer, tick, player);
    } else {
      buffer_.resize(buffer.frames(), buffer.channels());
      source_a_->step(buffer, tick, player);
      source_b_->step(buffer_, tick, player);
      stk::StkFloat *dst = &(buffer[0]);
      stk::StkFloat *src = &(buffer_[0]);
      for (int i=buffer.size()-1; i>=0; i--) { *dst++ += *src++; }
    }
    return *this;
  }

  // If this was an array of sources:
  //   if all inputs null, return kIndefinite
  //   if any input->getStart() == kIndefinite, return kIndefinite
  //   else return min(inputs->getStart())
  //
  Tick MixStream::getStart() {
    if ((source_a_ == NULL) && (source_b_ == NULL)) {
      return kIndefinite;
    } else if (source_b_ == NULL) {
      return source_a_->getStart();
    } else if (source_a_ == NULL) {
      return source_b_->getStart();
    } else if ((source_a_->getStart() == kIndefinite) || (source_b_->getStart() == kIndefinite)) {
      return kIndefinite;
    } else {
      return std::min(source_a_->getStart(), source_b_->getStart());
    }
  }

  Tick MixStream::getEnd() {
    if ((source_a_ == NULL) && (source_b_ == NULL)) {
      return kIndefinite;
    } else if (source_b_ == NULL) {
      return source_a_->getEnd();
    } else if (source_a_ == NULL) {
      return source_b_->getEnd();
    } else if ((source_a_->getEnd() == kIndefinite) || (source_b_->getEnd() == kIndefinite)) {
      return kIndefinite;
    } else {
      return std::max(source_a_->getEnd(), source_b_->getEnd());
    }
  }

}
