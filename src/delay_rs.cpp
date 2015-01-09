#include "delay_rs.h"

namespace mu {

  DelayRS::DelayRS()
    : source_ ( NULL ), delay_ ( 0 ) {
  }

  DelayRS::~DelayRS() {
  }

  void DelayRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (source_ != NULL) {
      source_->render(frames, base_tick - delay_, start_tick - delay_, end_tick - delay_);
    }
  }

}
