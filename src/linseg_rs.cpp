#include "linseg_rs.h"

namespace mu {

  void LinsegRS::render(stk::StkFrames& frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (breakpoints_.size() == 0) {
      return;

    } else if (breakpoints_.size() == 1) {
      MuTick tick = breakpoints_.begin()->first;
      if ((tick >= start_tick) && (tick < end_tick)) {
        MuFloat value = breakpoints_.begin()->second;
        for (int ch=frames.channels()-1; ch>=0; ch--) {
          frames(frame_index(base_tick, tick), ch) = value;
        }
      }
      return;

    } else {                    // two or more breakpoints
      MuTick s0 = std::max(start_tick, breakpoints_.begin()->first);
      MuTick e0 = std::min(end_tick, breakpoints_.rbegin()->first);
      for (MuTick tick=s0; tick<e0; tick++) {
        MuFloat value = lerp(tick);
        for (int ch=frames.channels()-1; ch>=0; ch--) {
          frames(frame_index(base_tick, tick), ch) = value;
        }
      }
      return;

    }

  }

  // Linear interpolate between breakpoints.  tick is guaranteed to
  // lie between breakpoints_.begin() and breakpoints_.end().
  MuFloat LinsegRS::lerp(MuTick tick) {
    Breakpoints::iterator ilo, ihi;
    ihi = breakpoints_.upper_bound(tick);
    ilo = ihi;
    ilo--;                      // isn't there a better way?
    return ilo->second + (ihi->second - ilo->second) * (tick - ilo->first) / (ihi->first - ilo->first);
  }


}
