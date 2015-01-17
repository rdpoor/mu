#include "delay_rs.h"

namespace mu {

  DelayRS::DelayRS()
    : source_ ( NULL ), delay_ ( 0 ) {
  }

  DelayRS::~DelayRS() {
  }

  bool DelayRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (source_ == NULL) return false;

    // printf("DelayRS::render(%p, %ld, %ld, %ld) delay=%ld\n", &frames, base_tick, start_tick, end_tick, delay_);
    return source_->render(frames, base_tick - delay_, start_tick - delay_, end_tick - delay_);
  }

}
