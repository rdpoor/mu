#include "reverb_sp.h"

namespace mu {

  ReverbSP::ReverbSP( void )
  : reverb_ ( NULL ) {
    setRatio(1.0);
  }
  
  ReverbSP::~ReverbSP( void ) {
    delete reverb_;
    reverb_ = NULL;
  }

  void ReverbSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }
    
  void ReverbSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (source_ == NULL) {
      zeroBuffer (buffer);
    } else {
      setup();
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      buffer_.resize(frame_count, channel_count);

      source_->step(buffer_, tick, is_new_event);
      for (int frame=0; frame<frame_count; frame++) {
        stk::StkFloat x = buffer_(frame, 0); // input from channel 0 only
        reverb_->tick(x);
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = (dry_ * x) + (wet_ * reverb_->lastOut(channel));
        }
      }
    }
  }

  Tick ReverbSP::getEnd() {
    if ((source_ == NULL) || (source_->getEnd() == TickUtils::indefinite())) {
      return TickUtils::indefinite();
    } else {
      // badly written to extend end time by reverb tail
      return source_->getEnd() + 44100 * 2.5;
    }
  }

} // namespace mu
