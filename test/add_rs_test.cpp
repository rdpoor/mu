#include "add_rs.h"
#include "dirac_rs.h"
#include "identity_rs.h"
#include "mu_types.h"

#include "frames_fixture.h"
#include "gtest/gtest.h"


class AddRSFixture : public FramesFixture {
protected:
  virtual void SetUp() {
    FramesFixture::SetUp();
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    bool r = add_rs_.render(frames_, base_tick, start_tick, end_tick);
    printf("AddRSFixture::Render(%ld, %ld, %ld) => %d\n", base_tick, start_tick, end_tick, r);
  }

  mu::AddRS add_rs_;
};


class AddRSZeroSources : public AddRSFixture {
protected:
  virtual void SetUp() {
    AddRSFixture::SetUp();
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

TEST_F(AddRSZeroSources, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSZeroSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSZeroSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}

// ================================================================
class AddRSOneSource : public AddRSFixture {
protected:
  virtual void SetUp() {
    AddRSFixture::SetUp();
    add_rs_.add_source(&identity_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = i + guard_value();
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
};



TEST_F(AddRSOneSource, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSOneSource, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSOneSource, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}

// ================================================================
class AddRSTwoSources : public AddRSFixture {
protected:
  virtual void SetUp() {
    AddRSFixture::SetUp();
    add_rs_.add_source(&dirac_rs_);
    add_rs_.add_source(&identity_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        // output should equal 1.0 when dirac's tick = 0, tick otherwise
        expected = (i == 0) ? guard_value() + i + 1.0 : guard_value() + i;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::DiracRS dirac_rs_;
  mu::IdentityRS identity_rs_;
};


TEST_F(AddRSTwoSources, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSTwoSources, RenderShifted) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(AddRSTwoSources, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}


