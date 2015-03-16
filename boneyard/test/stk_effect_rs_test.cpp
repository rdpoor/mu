#include "mu_types.h"
#include "gtest/gtest.h"
#include "stk_effect_rs.h"
#include "identity_rs.h"
#include "frames_fixture.h"
#include <stdio.h>

// define a simple subclass of StkEffectRS
namespace mu {
  class StkInvertRS : public StkEffectRS {
  public:
    void tick(MuFloat value) {
      // printf("StkInvertRS value = %f\n", value);
      last_out_ = -value;
    }
    MuFloat last_out(int channel) {
      (void)channel;            // ignore silently
      // printf("StkInvertRS last_out_ = %f\n", last_out_);
      return last_out_;
    }
  protected:
    MuFloat last_out_;
  };
}


class StkEffectRS : public FramesFixture {
protected:
  virtual void SetUp() {
    FramesFixture::SetUp();
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat blend) {
    Render(base_tick, start_tick, end_tick, blend);
    Verify(base_tick, start_tick, end_tick, blend);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat blend) {
    // printf("Render %ld, %ld, %ld, %f\n", base_tick, start_tick, end_tick, blend);
    stk_invert_rs_.set_source(&identity_rs_);
    stk_invert_rs_.set_blend(blend);
    stk_invert_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuFloat blend) {
    // printf("Verify %ld, %ld, %ld, %f\n", base_tick, start_tick, end_tick, blend);
    unsigned int n_channels = frames_.channels();

    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i>=start_tick) && (i<end_tick)) {
        expected = (-1.0 * blend * i) + ((1.0 - blend) * i);
      }
      for (unsigned int j=0; j<n_channels; j++) {
        mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_EQ(expected, actual) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
  mu::StkInvertRS stk_invert_rs_;
};


TEST_F(StkEffectRS, Render100) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(StkEffectRS, Render000) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 0.0);
}

TEST_F(StkEffectRS, RenderOffset100) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(StkEffectRS, RenderOffset000) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 0.0);
}

TEST_F(StkEffectRS, RenderSubset100) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 1.0);
}

TEST_F(StkEffectRS, RenderSubset000) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 0.0);
}
