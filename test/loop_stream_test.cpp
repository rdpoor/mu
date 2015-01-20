#include "gtest/gtest.h"
#include "loop_stream.h"
#include "dirac_stream.h"
#include "buffer_fixture.h"

using namespace mu;

class LoopStreamTest : public BufferFixture {
protected:
  void SetUp() {
    BufferFixture::SetUp();     // buffer_ filled with guard_value()
  }

  void RunTest(MuTick source_start, 
               MuTick source_end, 
               MuTick interval, 
               MuTick buffer_start) {
    loop_stream_.set_source(&dirac_stream_);
    loop_stream_.set_source_start(source_start);
    loop_stream_.set_source_end(source_end);
    loop_stream_.set_interval(interval);

    loop_stream_.render(buffer_, buffer_start);
  }

  void Verify(MuTick interval, MuTick buffer_start, MuFloat overlap) {
    int n_frames = buffer_.frames();
    int n_channels = buffer_.channels();

    for (int frame=n_frames-1; frame >= 0; frame--) {
      MuTick tick = frame + buffer_start;
      MuFloat expected = ((tick % interval) == 0) ? overlap : 0.0;
      for (int channel=n_channels-1; channel >= 0; channel--) {
        MuFloat value = buffer_(frame, channel);
        ASSERT_DOUBLE_EQ(expected, value) << 
          "at (frame, channel)[tick]=(" << frame << 
          ", " << channel << 
          ")[" << tick << "]";
      }
    }
  }

  DiracStream dirac_stream_;
  LoopStream loop_stream_;
};

TEST_F(LoopStreamTest, Test01) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

TEST_F(LoopStreamTest, Test02) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

TEST_F(LoopStreamTest, Test03) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);

  buffer_start += buffer_.frames();
  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

TEST_F(LoopStreamTest, Test04) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);

  buffer_start += buffer_.frames();
  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

// Test source stream with overlap.
//
// TODO: DiracStream isn't particularly good for verifying overlap.  Use
// IdentityStream instead.
TEST_F(LoopStreamTest, Test05) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

TEST_F(LoopStreamTest, Test06) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);

  buffer_start += buffer_.frames();
  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

TEST_F(LoopStreamTest, Test07) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);

  buffer_start += buffer_.frames();
  RunTest(source_start, source_end, interval, buffer_start);
  Verify(interval, buffer_start, 1.0);
}

