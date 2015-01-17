#include "sine_rs.h"
#include <math.h>

namespace mu {

  bool SineRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    for (MuTick i=start_tick; i<end_tick; i++) {
      double u = (double)(i) / frames.dataRate();
      double v = amplitude_ * sin((u * 2.0 * M_PI * frequency_) + phase_);
      for (int j = 0; j < frames.channels(); j++) {
        frames(i-base_tick, j) = v;
      }
    }
    return true;
  }


}
