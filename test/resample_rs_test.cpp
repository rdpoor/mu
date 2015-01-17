#include "mu_types.h"
#include "gtest/gtest.h"
#include "resample_rs.h"
#include "sine_rs.h"
#include "linseg_rs.h"
#include "frames_fixture.h"
#include <cmath>

#define GAIN 0.31622776601683794
#define FREQ 440.0
#define TOLERANCE 0.000001

class ResampleRS : public FramesFixture {
protected:
  virtual void SetUp() {
    FramesFixture::SetUp();

    sine_rs_.set_frequency(FREQ);
    sine_rs_.set_amplitude(GAIN);
    sine_rs_.set_phase(0.0);

    resample_rs_.set_timing_source(&linseg_rs_);
    resample_rs_.set_sample_source(&sine_rs_);
  }

  void render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    resample_rs_.render(frames_, base_tick, start_tick, end_tick);
  }

  void verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    stk::StkFrames timing_buffer;
    timing_buffer.resize(frames_.frames(), frames_.channels());
    linseg_rs_.render(timing_buffer, base_tick, start_tick, end_tick);

    double omega = (FREQ * 2.0 * M_PI) / frames_.dataRate();

    // mu::MuFloat f = frames_(frames_.size(), 0); // expect error catch here

    for (mu::MuTick tick = start_tick; tick < end_tick; tick += 1) {
      mu::MuFloat tau = timing_buffer(mu::RenderStream::frame_index(base_tick, tick), 0);
      // printf("timing_buffer[%ld] (tick=%ld) = %f\n", mu::RenderStream::frame_index(base_tick, tick), tick, tau);
      mu::MuFloat expected = GAIN * sin(tau * omega);
      for (int channel = frames_.channels()-1; channel >= 0; channel--) {
        mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), channel);
        ASSERT_NEAR(expected, actual, TOLERANCE);
      }
    }
  }

  mu::ResampleRS resample_rs_;
  mu::SineRS sine_rs_;
  mu::LinsegRS linseg_rs_;

};

TEST_F(ResampleRS, Identity) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  linseg_rs_.clear();
  linseg_rs_.add_breakpoint(start_tick, start_tick);
  linseg_rs_.add_breakpoint(end_tick, end_tick);
  render(base_tick, start_tick, end_tick);
  verify(base_tick, start_tick, end_tick);
}

TEST_F(ResampleRS, NegativeIdentity) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  linseg_rs_.clear();
  linseg_rs_.add_breakpoint(start_tick, end_tick);
  linseg_rs_.add_breakpoint(end_tick, start_tick);
  render(base_tick, start_tick, end_tick);
  verify(base_tick, start_tick, end_tick);
}

TEST_F(ResampleRS, TwoX) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  linseg_rs_.clear();
  linseg_rs_.add_breakpoint(start_tick, start_tick * 2.0);
  linseg_rs_.add_breakpoint(end_tick, end_tick * 2.0);
  render(base_tick, start_tick, end_tick);
  verify(base_tick, start_tick, end_tick);
}
