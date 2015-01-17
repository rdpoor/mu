#include "gtest/gtest.h"
#include "constant_rs.h"
#include "frames_fixture.h"

class ConstantRS : public FramesFixture {
protected:
  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat value) {
    Render(base_tick, start_tick, end_tick, value);
    Verify(base_tick, start_tick, end_tick, value);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat value) {
    constant_rs_.set_value(value);
    constant_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat value) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = value;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, actual) << "at (frame, channel)=(" << i << ", " << ")";
      }
    }
  }

  mu::ConstantRS constant_rs_;
};


TEST_F(ConstantRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 22.0);
}

TEST_F(ConstantRS, RenderOffset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 22.0);
}

TEST_F(ConstantRS, RenderSubset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick+1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 22.0);
}
