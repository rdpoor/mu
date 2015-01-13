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

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    multiply_rs_.set_scale(scale);
    multiply_rs_.render(frames_, base_tick, start_tick, end_tick);
  }

  mu::MultiplyRS multiply_rs_;
};


class MultiplyRSZeroSources : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    Render(base_tick, start_tick, end_tick, scale);
    Verify(base_tick, start_tick, end_tick, scale);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = scale;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

};

TEST_F(MultiplyRSZeroSources, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSZeroSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSZeroSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

// ================================================================
class MultiplyRSOneSource : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
    multiply_rs_.add_source(&identity_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    Render(base_tick, start_tick, end_tick, scale);
    Verify(base_tick, start_tick, end_tick, scale);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = i * scale;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
};



TEST_F(MultiplyRSOneSource, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSOneSource, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSOneSource, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSOneSource, RenderScaled) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 2.0);
}

// ================================================================
class MultiplyRSTwoSources : public MultiplyRSFixture {
protected:
  virtual void SetUp() {
    MultiplyRSFixture::SetUp();
    multiply_rs_.add_source(&identity_rs1_);
    multiply_rs_.add_source(&identity_rs2_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    Render(base_tick, start_tick, end_tick, scale);
    Verify(base_tick, start_tick, end_tick, scale);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat scale) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = i * i * scale;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
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

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSTwoSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSTwoSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(MultiplyRSTwoSources, RenderScaled) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 2.0);
}

