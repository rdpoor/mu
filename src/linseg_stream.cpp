#include "linseg_stream.h"

namespace mu {

  // compute one value.  breakpoints_ is guaranteed to have at least
  // one breakpoint.
  // TODO: vectorize.
  stk::StkFloat LinsegStream::lerp(Tick tick) {
    if (tick < breakpoints_.begin()->first) {
      return hold_value_ ? breakpoints_.begin()->second : 0.0;

    } else if (tick == breakpoints_.begin()->first) {
      return breakpoints_.begin()->second;

    } else if (tick == breakpoints_.rbegin()->first) {
      return breakpoints_.rbegin()->second;

    } else if (tick > breakpoints_.rbegin()->first) {
      return hold_value_ ? breakpoints_.rbegin()->second : 0.0;

    } else {
      // tick is guaranteed to lie within two breakpoints
      Breakpoints::iterator cursor = breakpoints_.lower_bound(tick);
      Tick x1 = cursor->first;
      stk::StkFloat y1 = cursor->second;
      --cursor;
      Tick x0 = cursor->first;
      stk::StkFloat y0 = cursor->second;

      return y0 + (tick - x0)*(y1-y0)/(x1-x0);
    }

  }

  LinsegStream& LinsegStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    if (breakpoints_.size() == 0) {
      zero_buffer(buffer);

    } else {
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      for (int frame=0; frame<frame_count; frame++) {
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = lerp(tick + frame);
        }
      }
    }

    return *this;
  }

  Tick LinsegStream::getStart() {
    if (breakpoints_.size() == 0) {
      return mu::kIndefinite;
    } else {
      return breakpoints_.begin()->first;
    }
  }

  Tick LinsegStream::getEnd() {
    if (breakpoints_.size() == 0) {
      return mu::kIndefinite;
    } else {
      return breakpoints_.rbegin()->first;
    }
  }

  LinsegStream& LinsegStream::addBreakpoint(Tick tick, stk::StkFloat value) {
    breakpoints_[tick] = value;
    return *this;
  }

  LinsegStream& LinsegStream::removeAllBreakpoints() {
    breakpoints_.clear();
    return *this;
  }

}
