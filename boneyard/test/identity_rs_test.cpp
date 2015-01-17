#include "gtest/gtest.h"
#include "identity_rs.h"
#include "frames_fixture.h"

class IdentityRS : public FramesFixture {
protected:
  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    identity_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        // output should equal tick
        expected = i;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
};


TEST_F(IdentityRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(IdentityRS, RenderOffset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(IdentityRS, RenderSubset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick+1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}
