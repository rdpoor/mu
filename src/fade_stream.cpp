#include "fade_stream.h"

namespace mu {

  FadeStream::FadeStream() : 
    start_ (kIndefinite), 
    end_ (kIndefinite),
    fade_time_ ( 0 ) {
    // pre-allocate transfer buffer
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  FadeStream::~FadeStream() {
    TRACE("FadeStream::~FadeStream()\n");
  }

  void FadeStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); 
    ss << "getFadeTIme() = " << getFadeTime() << std::endl;
    inspectIndent(ss, level); 
    ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  FadeStream& FadeStream::step(stk::StkFrames& buffer, 
                                 Tick tick, 
                                 Player& player) {
    if (source_ == NULL) {
      zeroBuffer(buffer);
      return *this;
    }

    Tick buf_s = tick;
    Tick buf_e = tick + buffer.frames();

    // a series of useful x values
    // Tick t0 = LONG_MIN;         // before everything
    Tick t1 = LONG_MIN;         // begining of fade in
    Tick t2 = LONG_MIN;         // end of fade in
    Tick t3 = LONG_MAX;         // beginning of fade out
    Tick t4 = LONG_MAX;         // end of fade out
    // Tick t5 = LONG_MAX;         // after everything

    Tick ss = source_->getStart();
    Tick se = source_->getEnd();

    // DRY alert: see getStart() and getEnd()
    if ((start_ == kIndefinite) && (ss == kIndefinite)) {
      t1 = LONG_MIN;
      t2 = LONG_MIN;
    } else if (start_ == kIndefinite) {
      t1 = ss;
      t2 = ss + fade_time_;
    } else if (ss == kIndefinite) {
      t1 = start_ - (fade_time_ / 2);
      t2 = t1 + fade_time_;
    } else {
      t1 = std::max(ss, start_ - (fade_time_ / 2));
      t2 = t1 + fade_time_;
    }

    if ((end_ == kIndefinite) && (se == kIndefinite)) {
      t4 = LONG_MAX;
      t3 = LONG_MAX;
    } else if (end_ == kIndefinite) {
      t4 = se;
      t3 = se - fade_time_;
    } else if (se == kIndefinite) {
      t4 = end_ + (fade_time_ / 2);
      t3 = t4 - fade_time_;
    } else {
      t4 = std::min(se, end_ + (fade_time_ / 2));
      t3 = t4 - fade_time_;
    }

    // change in gain per sample
    double dGdT = (fade_time_ == 0) ? 0.0 : 1.0 / fade_time_;

    // We need to handle the case where the fade out starts before
    // the fade in completes.
    if ((t1 != kIndefinite) && (t4 != kIndefinite)) {
      if (t1 >= t4) {
        // can this happen?  Never fade in...
        t1 = LONG_MIN;
        t2 = LONG_MIN;
        t3 = LONG_MAX;
        t4 = LONG_MAX;
      } else if (t2 >= t3) {
        // fade out starts before fade in completes
        t2 = t3 = (t2 + t3) / 2;
      }
    }

    // At this point, t0..t5 descibe all the breakpoints.
    Tick x0, x1, dx;

    // Write zeros between t0 and t1
    x0 = buf_s;                 // std::max(t0, buf_s);
    x1 = std::min(t1, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some zeros get written in this buffer.
      // TODO: abstract out and optimize this loop
      long int i, x;
      for (i=0, x=x0; i<dx; i++, x++) {
        for (long int j=0; j<buffer.channels(); j++) {
          buffer(x, j) = 0.0;
        }
      }
      if (x1 == buf_e) return *this;
    }

    // Ramp gain up between t1 (inclusive) and t2 (exclusive)
    x0 = std::max(t1, buf_s);
    x1 = std::min(t2, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some ramp up happens in this buffer.
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, player);

      long int x, i;
      for (i=0, x=x0; i<dx; i++, x++) {
        // gain is 0 when x=t1, ramps up as x increases
        double gain = dGdT * (x - t1);
        for (int j=0; j<buffer.channels(); j++) {
          buffer(x, j) = buffer_(i, j) * gain;
        }
      }
      if (x1 == buf_e) return *this;
    }

    // Copy source verbatim between t2 (inclusive) and t3 (exclusive)
    x0 = std::max(t2, buf_s);
    x1 = std::min(t3, buf_e);
    dx = x1 - x0;
    if (dx == buffer.frames()) {
      // copy buffer verbatim
      source_->step(buffer, tick, player);
      return *this;
    } else if (dx > 0) {
      // copy some frames verbatim
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, player);
      copyBuffer(buffer_, 0, buffer, x0-buf_s, dx);
      if (x1 == buf_e) return *this;
    }

    // Ramp gain down between t3 (inclusive) and t4 (exclusive)
    x0 = std::max(t3, buf_s);
    x1 = std::min(t4, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some ramp up happens in this buffer.
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, player);

      long int x, i;
      for (i=0, x=x0; i<dx; i++, x++) {
        // gain is zero when x = t4, ramps down towards there
        double g = dGdT * (t4 - x);
        for (int j=0; j<buffer.channels(); j++) {
          buffer(x, j) = buffer_(i, j) * g;
        }
      }
      if (x1 == buf_e) return *this;
    }
    
    // Write zeros between t4 and t5
    x0 = std::max(t4, buf_s);
    x1 = buf_e;                 // std::min(t5, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some zeros get written in this buffer.
      long int i, x;
      for (i=0, x=x0; i<dx; i++, x++) {
        for (long int j=0; j<buffer.channels(); j++) {
          buffer(x, j) = 0.0;
        }
      }
      if (x1 == buf_e) return *this;
    }

    // one of the above clauses should have filled out the buffer
    fprintf(stderr,
            "warning: at end of step, buf_s=%ld, buf_e=%ld, x0=%ld, x1=%ld\n",
            buf_s, buf_e, x0, x1);

    return *this;
  }
  
  Tick FadeStream::getStart() {
    Tick ss = (source_ == NULL) ? kIndefinite : source_->getStart();
    
    if ((start_ == kIndefinite) && (ss == kIndefinite)) {
      return kIndefinite;
    } else if (start_ == kIndefinite) {
      return ss;
    } else if (ss == kIndefinite) {
      return start_ - (fade_time_ / 2);
    } else {
      return std::max(ss, start_ - (fade_time_ / 2));
    }
  }

  Tick FadeStream::getEnd() {
    Tick se = (source_ == NULL) ? kIndefinite : source_->getEnd();

    if ((end_ == kIndefinite) && (se == kIndefinite)) {
      return kIndefinite;
    } else if (end_ == kIndefinite) {
      return se;
    } else if (se == kIndefinite) {
      return end_ + (fade_time_ / 2);
    } else {
      return std::min(se, end_ + (fade_time_ / 2));
    }
  }

}
