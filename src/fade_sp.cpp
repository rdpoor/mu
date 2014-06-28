/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/
#include "fade_sp.h"

// #define COVERAGE_CHECK(x) fprintf(stderr,x)
#define COVERAGE_CHECK(x)
// #define SEQUENCE_CHECK(x) fprintf(stderr,x)
#define SEQUENCE_CHECK(x)

namespace mu {

  FadeSP::FadeSP() : 
    start_ ( kIndefinite ), 
    end_ ( kIndefinite ),
    fade_time_ ( kDefaultFadeTime ) {
    // pre-allocate transfer buffer
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  FadeSP::~FadeSP() {
    TRACE("FadeSP::~FadeSP()\n");
  }

  void FadeSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); 
    ss << "getFadeTIme() = " << getFadeTime() << std::endl;
    inspectIndent(ss, level); 
    ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void FadeSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (source_ == NULL) {
      COVERAGE_CHECK("FadeSteam::step() A\n");
      zeroBuffer(buffer);
      return;
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
        COVERAGE_CHECK("FadeSteam::step() B\n");
        t1 = LONG_MAX;
        t2 = LONG_MAX;
        t3 = LONG_MIN;
        t4 = LONG_MIN;
      } else if (t2 >= t3) {
        // fade out starts before fade in completes
        COVERAGE_CHECK("FadeSteam::step() C\n");
        t2 = t3 = (t2 + t3) / 2;
      }
    }

    // At this point, t0..t5 descibe all the breakpoints:
    // t0 <= t < t1: result is zero (not yet faded in)
    // t1 <= t < t2: fade in linearly over fade_time_ samples
    // t2 <= t < t3: fully faded in
    // t3 <= t < t4: fade out linearly over fade_time_ samples
    // t4 <= t < t5: result is zero (fully faded out)

    Tick x0, x1, dx;

    // fprintf(stderr, "%ld, %ld: %ld, %ld, %ld, %ld\n", buf_s, buf_e, t1, t2, t3, t4);

    // Write zeros between t0 and t1
    x0 = buf_s;                 // std::max(t0, buf_s);
    x1 = std::min(t1, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some zeros get written in this buffer.
      // TODO: abstract out and optimize this loop
      // TODO: optimize for dx == buffer.frames()
      SEQUENCE_CHECK("A");
      COVERAGE_CHECK("FadeSteam::step() D\n");
      long int i, x;
      for (i=0, x=x0; i<dx; i++, x++) {
        for (long int j=0; j<buffer.channels(); j++) {
          buffer(x-buf_s, j) = 0.0;
        }
      }
      if (x1 == buf_e) return;
    }

    // Ramp gain up between t1 (inclusive) and t2 (exclusive)
    x0 = std::max(t1, buf_s);
    x1 = std::min(t2, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some ramp up happens in this buffer.
      SEQUENCE_CHECK("B");
      COVERAGE_CHECK("FadeSteam::step() E\n");
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, is_new_event);

      // fprintf(stderr,"x0=%ld, x1=%ld, dx=%ld\n", x0, x1, dx);

      long int x, i;
      for (i=0, x=x0; i<dx; i++, x++) {
        // gain is 0 when x=t1, ramps up as x increases
        double gain = dGdT * (x - t1);
        for (int j=0; j<buffer.channels(); j++) {
          buffer(x-buf_s, j) = buffer_(i, j) * gain;
        }
      }
      if (x1 == buf_e) return;
    }

    // Copy source verbatim between t2 (inclusive) and t3 (exclusive)
    x0 = std::max(t2, buf_s);
    x1 = std::min(t3, buf_e);
    dx = x1 - x0;
    if (dx == buffer.frames()) {
      // copy buffer verbatim
      SEQUENCE_CHECK("C");
      COVERAGE_CHECK("FadeSteam::step() F\n");
      source_->step(buffer, tick, is_new_event);
      return;
    } else if (dx > 0) {
      // copy some frames verbatim
      SEQUENCE_CHECK("D");
      COVERAGE_CHECK("FadeSteam::step() G\n");
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, is_new_event);
      copyBuffer(buffer_, 0, buffer, x0-buf_s, dx);
      if (x1 == buf_e) return;
    }

    // Ramp gain down between t3 (inclusive) and t4 (exclusive)
    x0 = std::max(t3, buf_s);
    x1 = std::min(t4, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some ramp down happens in this buffer.
      SEQUENCE_CHECK("E");
      COVERAGE_CHECK("FadeSteam::step() H\n");
      buffer_.resize(dx, buffer.channels());
      source_->step(buffer_, x0, is_new_event);

      long int x, i;
      for (i=0, x=x0; i<dx; i++, x++) {
        // gain is zero when x = t4, ramps down towards there
        double gain = dGdT * (t4 - x);
        for (int j=0; j<buffer.channels(); j++) {
          buffer(x-buf_s, j) = buffer_(i, j) * gain;
        }
      }
      if (x1 == buf_e) return;
    }
    
    // Write zeros between t4 and t5
    x0 = std::max(t4, buf_s);
    x1 = buf_e;                 // std::min(t5, buf_e);
    dx = x1 - x0;
    if (dx > 0) {
      // some zeros get written in this buffer.
      // TODO: optimize for dx == buffer.frames()
      long int i, x;
      SEQUENCE_CHECK("F");
      COVERAGE_CHECK("FadeSteam::step() I\n");
      for (i=0, x=x0; i<dx; i++, x++) {
        for (long int j=0; j<buffer.channels(); j++) {
          buffer(x-buf_s, j) = 0.0;
        }
      }
      if (x1 == buf_e) return;
    }

    // one of the above clauses should have filled out the buffer
    fprintf(stderr,
            "warning: at end of step, buf_s=%ld, buf_e=%ld, x0=%ld, x1=%ld\n",
            buf_s, buf_e, x0, x1);
  }
  
  Tick FadeSP::getStart() {
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

  Tick FadeSP::getEnd() {
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
