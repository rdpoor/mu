#include "mu_types.h"
#include "gtest/gtest.h"
#include "identity_rs.h"
#include "crop_rs.h"
#include "frames_fixture.h"

class CropRS : public FramesFixture {
protected:
  static const mu::MuTick kCrop = 25;

  virtual void SetUp() {
    FramesFixture::SetUp();
    // Set up Identity function feeding a Crop
    crop_rs_.set_source(&identity_rs_);
  }

  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuTick crop_start, mu::MuTick crop_end) {
    crop_rs_.set_crop_start(crop_start);
    crop_rs_.set_crop_end(crop_end);
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick, crop_start, crop_end);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    crop_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick, mu::MuTick crop_start, mu::MuTick crop_end) {
    for (mu::MuTick i=base_tick; i<base_tick + frames_.frames(); i++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((i >= start_tick) && (i < end_tick) && (i >= crop_start) && (i < crop_end)) {
        expected = i;
      }
      for (unsigned int j=0; j<frames_.channels(); j++) {
        mu::MuFloat value = frames_(mu::RenderStream::frame_index(base_tick, i), j);
        ASSERT_DOUBLE_EQ(expected, value) << "at (frame, channel)=(" << i << ", " << j << ")";
      }
    }
  }

  mu::IdentityRS identity_rs_;
  mu::CropRS crop_rs_;
};


TEST_F(CropRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 0, frames_.frames());
}

TEST_F(CropRS, RenderCropped) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1, frames_.frames()-1);
}

TEST_F(CropRS, RenderOffset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 0, frames_.frames());
}

TEST_F(CropRS, RenderOffsetCropped) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, 1, frames_.frames()-1);
}

TEST_F(CropRS, RenderSubset) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 0, frames_.frames());
}

TEST_F(CropRS, RenderSubsetCropped) {
  mu::MuTick base_tick = 19;
  mu::MuTick start_tick = base_tick + 1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick, 1, frames_.frames()-1);
}

TEST_F(CropRS, RenderSplitLo) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();
  mu::MuTick crop_start = (start_tick + end_tick) / 2;
  mu::MuTick crop_end = crop_start + frames_.frames();

  RunTest(base_tick, start_tick, end_tick, crop_start, crop_end);
}

TEST_F(CropRS, RenderSplitHi) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();
  mu::MuTick crop_end = (start_tick + end_tick) / 2;
  mu::MuTick crop_start = crop_end - frames_.frames();

  RunTest(base_tick, start_tick, end_tick, crop_start, crop_end);
}
