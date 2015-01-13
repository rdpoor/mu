#include "mu_types.h"
#include "gtest/gtest.h"
#include "dirac_rs.h"
#include "loop_rs.h"
#include "frames_fixture.h"


class LoopRS : public FramesFixture {
protected:

  virtual void SetUp() {
    FramesFixture::SetUp();
    // Set up Dirac function feeding a Loop
    loop_rs_.set_source(&dirac_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuTick duration) {
    loop_rs_.set_duration(duration);
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick, duration);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    loop_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuTick duration) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        // output should equal 1.0 when tick MOD duration = 0
        expected = ((i % duration) == 0) ? 1.0 : 0.0;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << ")";
      }
    }
  }

  mu::DiracRS dirac_rs_;
  mu::LoopRS loop_rs_;
};


TEST_F(LoopRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 3);
  RunTest(base_tick, start_tick, end_tick, 32);
  RunTest(base_tick, start_tick, end_tick, 27);
}

TEST_F(LoopRS, RenderOffset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 3);
  RunTest(base_tick, start_tick, end_tick, 32);
  RunTest(base_tick, start_tick, end_tick, 27);
}

TEST_F(LoopRS, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 3);
  RunTest(base_tick, start_tick, end_tick, 32);
  RunTest(base_tick, start_tick, end_tick, 27);
}
