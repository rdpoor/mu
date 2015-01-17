#include "mu_types.h"
#include "gtest/gtest.h"
#include "dirac_rs.h"
#include "delay_rs.h"
#include "frames_fixture.h"


class DelayRS : public FramesFixture {
protected:
  static const mu::MuTick kDelay = 25;

  virtual void SetUp() {
    FramesFixture::SetUp();
    // Set up Dirac function feeding a Delay
    delay_rs_.set_source(&dirac_rs_);
    delay_rs_.set_delay(kDelay);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    delay_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        // output should equal 1.0 only when dirac's tick = 0
        expected = (i == kDelay) ? 1.0 : 0.0;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << ")";
      }
    }
  }

  mu::DiracRS dirac_rs_;
  mu::DelayRS delay_rs_;
};


TEST_F(DelayRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(DelayRS, RenderOffset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(DelayRS, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}
