#include "mu_types.h"
#include "gtest/gtest.h"
#include "linseg_rs.h"
#include "frames_fixture.h"


class LinsegRS : public FramesFixture {
protected:
  mu::LinsegRS linseg_rs_;
};


TEST_F(LinsegRS, WithZeroBreakpoints) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  linseg_rs_.render(frames_, base_tick, start_tick, end_tick);

  for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
    mu::MuFloat expected = FramesFixture::guard_value();
    for (unsigned int ch=0; ch<frames_.channels(); ch++) {
      mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
      ASSERT_EQ(expected, actual) << "at (tick, channel)=(" << tick << ", " << ch << ")";
    }
  }
}

TEST_F(LinsegRS, WithOneBreakpoint) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  mu::MuTick break0_tick = (base_tick + end_tick) / 2;
  mu::MuFloat break0_value = 100.0;
  linseg_rs_.add_breakpoint(break0_tick, break0_value);

  linseg_rs_.render(frames_, base_tick, start_tick, end_tick);

  for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
    mu::MuFloat expected = FramesFixture::guard_value();
    // a LinsegRS with one breakpoint acts like a dirac function
    if (tick == break0_tick) {
      expected = break0_value;
    }
    for (unsigned int ch=0; ch<frames_.channels(); ch++) {
      mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
      ASSERT_EQ(expected, actual) << "at (tick, channel)=(" << tick << ", " << ch << ")";
    }
  }
}

TEST_F(LinsegRS, WithTwoBreakpoints) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  mu::MuTick break0_tick = (base_tick + end_tick) / 4.0;
  mu::MuFloat break0_value = 100.0;
  mu::MuTick break1_tick = (base_tick + end_tick) * 3.0 / 4.0;
  mu::MuFloat break1_value = 1.0;
  linseg_rs_.add_breakpoint(break0_tick, break0_value);
  linseg_rs_.add_breakpoint(break1_tick, break1_value);

  linseg_rs_.render(frames_, base_tick, start_tick, end_tick);

  for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
    mu::MuFloat expected = FramesFixture::guard_value();
    if ((tick >= break0_tick) && (tick < break1_tick)) {
      expected = break0_value + (break1_value - break0_value) * (tick - break0_tick) / (break1_tick - break0_tick);
    }
    for (unsigned int ch=0; ch<frames_.channels(); ch++) {
      mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
      ASSERT_DOUBLE_EQ(expected, actual) << "at (tick, channel)=(" << tick << ", " << ch << ")";
    }
  }
}

TEST_F(LinsegRS, BeforeTwoBreakpoints) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  mu::MuTick break0_tick = end_tick + (base_tick + end_tick) / 4.0;
  mu::MuFloat break0_value = 100.0;
  mu::MuTick break1_tick = end_tick + (base_tick + end_tick) * 3.0 / 4.0;
  mu::MuFloat break1_value = 1.0;
  linseg_rs_.add_breakpoint(break0_tick, break0_value);
  linseg_rs_.add_breakpoint(break1_tick, break1_value);

  linseg_rs_.render(frames_, base_tick, start_tick, end_tick);

  for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
    mu::MuFloat expected = FramesFixture::guard_value();
    if ((tick >= break0_tick) && (tick <= break1_tick)) {
      expected = break0_value + (break1_value - break0_value) * (tick - break0_tick) / (break1_tick - break0_tick);
    }
    for (unsigned int ch=0; ch<frames_.channels(); ch++) {
      mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
      ASSERT_DOUBLE_EQ(expected, actual) << "at (tick, channel)=(" << tick << ", " << ch << ")";
    }
  }
}

TEST_F(LinsegRS, AfterTwoBreakpoints) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  mu::MuTick break0_tick = ((base_tick + end_tick) / 4.0) - end_tick;
  mu::MuFloat break0_value = 100.0;
  mu::MuTick break1_tick = ((base_tick + end_tick) * 3.0 / 4.0) - end_tick;
  mu::MuFloat break1_value = 1.0;
  linseg_rs_.add_breakpoint(break0_tick, break0_value);
  linseg_rs_.add_breakpoint(break1_tick, break1_value);

  linseg_rs_.render(frames_, base_tick, start_tick, end_tick);

  for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
    mu::MuFloat expected = FramesFixture::guard_value();
    if ((tick >= break0_tick) && (tick <= break1_tick)) {
      expected = break0_value + (break1_value - break0_value) * (tick - break0_tick) / (break1_tick - break0_tick);
    }
    for (unsigned int ch=0; ch<frames_.channels(); ch++) {
      mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
      ASSERT_DOUBLE_EQ(expected, actual) << "at (tick, channel)=(" << tick << ", " << ch << ")";
    }
  }
}

