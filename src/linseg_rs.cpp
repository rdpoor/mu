#include "linseg_rs.h"

namespace mu {

  bool LinsegRS::render(stk::StkFrames& frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (breakpoints_.size() == 0) {
      // no breakpoints, no effect
      return false;

    } else if (breakpoints_.size() == 1) {
      // one breakpoint: emit value at that tick (only)
      bool anything_rendered = false;
      MuTick tick = breakpoints_.begin()->first;
      if ((tick >= start_tick) && (tick < end_tick)) {
        MuFloat value = breakpoints_.begin()->second;
        anything_rendered = true;
        for (int ch=frames.channels()-1; ch>=0; ch--) {
          frames(frame_index(base_tick, tick), ch) = value;
        }
      }
      return anything_rendered;

    } else {
      // two or more breakpoints
      bool anything_rendered = false;

      MuTick s0 = std::max(start_tick, breakpoints_.begin()->first);
      MuTick e0 = std::min(end_tick, breakpoints_.rbegin()->first);
      int n_channels = frames.channels();
      for (MuTick tick=s0; tick < e0; tick++) {
        MuFloat value = lerp(tick);
        anything_rendered = true;
        for (int ch=n_channels-1; ch>=0; ch--) {
          frames(frame_index(base_tick, tick), ch) = value;
        }
      }
      return anything_rendered;

    }

  }

  // Linear interpolate between breakpoints.  tick is guaranteed to
  // lie between breakpoints_.begin() and breakpoints_.end().
  //
  // TODO: consider writing a custom finder that uses the previous
  // iterator, since 99% of the time we're searching for the same
  // breakpoint.
  MuFloat LinsegRS::lerp(MuTick tick) {
    Breakpoints::iterator ilo, ihi;
    ihi = breakpoints_.upper_bound(tick);
    ilo = ihi;
    ilo--;                      // isn't there a better way?
    return ilo->second + (ihi->second - ilo->second) * (tick - ilo->first) / (ihi->first - ilo->first);
  }


}
