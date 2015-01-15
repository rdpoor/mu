#include "multiply_rs.h"
#include "identity_rs.h"
#include "mu_types.h"

#include "frames_fixture.h"
#include "gtest/gtest.h"


class MultiplyRSFixture : public FramesFixture {
protected:
  virtual void SetUp() {
    FramesFixture::SetUp();
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    multiply_rs_.render(frames_, base_tick, start_tick, end_tick);
  }

  mu::MultiplyRS multiply_rs_;
};


class MultiplyRSZeroSources : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    (void)start_tick;           // suppress compiler warnings
    (void)end_tick;

    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

};

TEST_F(MultiplyRSZeroSources, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSZeroSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSZeroSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}

// ================================================================
class MultiplyRSOneSource : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
    multiply_rs_.add_source(&identity_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = i * guard_value();
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
};



TEST_F(MultiplyRSOneSource, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSOneSource, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSOneSource, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}

// ================================================================
class MultiplyRSTwoSources : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
    multiply_rs_.add_source(&identity_rs1_);
    multiply_rs_.add_source(&identity_rs2_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = guard_value() * i * i;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs1_;
  mu::IdentityRS identity_rs2_;
};


TEST_F(MultiplyRSTwoSources, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSTwoSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(MultiplyRSTwoSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}


