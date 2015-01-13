#include "gtest/gtest.h"
#include "file_read_rs.h"
#include "frames_fixture.h"
#include <cmath>

// The test tone is 440Hz calculated at 44100 fps stereo
// at -10db below full scale.
//
#define TEST_TONE_DIRECTORY "/Users/r/Projects/Mu/SoundSets/Tones/"
#define GAIN 0.31622776601683794
#define FREQ 440.0
#define TOLERANCE 0.0000001

class FileReadRS : public FramesFixture {
protected:
  void RunTest(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    file_read_rs_.set_file_name(TEST_TONE_DIRECTORY "069.wav");
    Render(base_tick, start_tick, end_tick);
    Verify(base_tick, start_tick, end_tick);
  }

  void Render(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    file_read_rs_.render(frames_, base_tick, start_tick, end_tick);
  }
    
  void Verify(mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick) {
    double omega = (FREQ * 2.0 * M_PI) / frames_.dataRate();

    for (mu::MuTick tick=base_tick; tick<base_tick + frames_.frames(); tick++) {
      mu::MuFloat expected = FramesFixture::guard_value();
      if ((tick>=start_tick) && (tick<end_tick)) {
        expected = GAIN * sin(tick * omega);
      }
      for (unsigned int ch=0; ch<frames_.channels(); ch++) {
        mu::MuFloat actual = frames_(mu::RenderStream::frame_index(base_tick, tick), ch);
        ASSERT_NEAR(expected, actual, TOLERANCE);
      }
    }
  }

  mu::FileReadRS file_read_rs_;
};


TEST_F(FileReadRS, Render) {
  mu::MuTick base_tick = 0;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(FileReadRS, RenderOffset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick;
  mu::MuTick end_tick = base_tick + frames_.frames();

  RunTest(base_tick, start_tick, end_tick);
}

TEST_F(FileReadRS, RenderSubset) {
  mu::MuTick base_tick = 1234;
  mu::MuTick start_tick = base_tick+1;
  mu::MuTick end_tick = base_tick + frames_.frames() - 1;

  RunTest(base_tick, start_tick, end_tick);
}
