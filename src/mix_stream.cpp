#include "mix_stream.h"

namespace mu {

  MixStream::~MixStream() {
    TRACE("MixStream::~MixStream()\n");
  }

  // TODO: See README notes about frameCount vs extent
  Tick MixStream::frameCount() {
    if (source_a_ == NULL) {
      if (source_b_ == NULL) {
        return kIndefinite;
      } else {
        return source_b_->frameCount();
      }
    } else {
      if (source_b_ == NULL) {
        return source_a_->frameCount();
      } else if (source_a_->frameCount() == kIndefinite) {
        return kIndefinite;
      } else if (source_b_->frameCount() == kIndefinite) {
        return kIndefinite;
      } else {
        return std::max(source_a_->frameCount(), source_b_->frameCount());
      }
    }
  }

  MixStream& MixStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    if (source_a_ == NULL) {
      if (source_b_ == NULL) {
        bzero(&(buffer[0]), buffer.frames() * buffer.channels() * sizeof(stk::StkFloat));
      } else {
        source_b_->step(buffer, tick, player);
      }
    } else {
      if (source_b_ == NULL) {
        source_a_->step(buffer, tick, player);
      } else {
        source_a_->step(buffer, tick, player);
        buffer_.resize(buffer.frames(), buffer.channels());
        source_b_->step(buffer_, tick, player);
        int sample_count = buffer.size();
        for (int i=0; i<sample_count; i++) { buffer[i] += buffer_[i]; }
      }
    }
    return *this;
  }

}
