#include "gtest/gtest.h"
#include "loop_stream.h"
#include "identity_stream.h"
#include "buffer_fixture.h"

using namespace mu;

class LoopStreamTest : public BufferFixture {
protected:
  void SetUp() {
    BufferFixture::SetUp();     // fill buffer_ with guard_value()
    identity_stream_ = new mu::IdentityStream();
    loop_stream_ = new mu::LoopStream();
  }

  void RunTest(MuTick source_start, 
               MuTick source_end, 
               MuTick interval, 
               MuTick buffer_start) {
    loop_stream_->set_source(identity_stream_);
    loop_stream_->set_source_start(source_start);
    loop_stream_->set_source_end(source_end);
    loop_stream_->set_interval(interval);

    loop_stream_->render(buffer_start, &buffer_);
  }

  MuFloat Expect(MuTick source_start, 
                 MuTick source_end, 
                 MuTick interval, 
                 MuTick tick) {
    // Expect() is written with the assumption that source_start = 0.  The
    // following ASSERT tests for an incorrectly written test, not a failing
    // implementation.
    // ASSERT_EQ(0, source_start);
    MuTick tick_m = tick % interval;
    if (source_end == interval) {
      return tick_m;
    } else if (source_end < interval) {
      return (tick_m < source_end) ? tick_m : 0.0;
    } else /* (source_end >= tick_m) */ {
      MuFloat total = 0.0;
      EXPECT_EQ(1, 0) << "implement me.";
      return total;
    }
  }

  void Verify(MuTick source_start, 
              MuTick source_end, 
              MuTick interval, 
              MuTick buffer_start) {
    int n_frames = buffer_.frames();
    int n_channels = buffer_.channels();

    for (int frame=n_frames-1; frame >= 0; frame--) {
      MuTick tick = frame + buffer_start;
      MuFloat expected = Expect(source_start, source_end, interval, tick);
      for (int channel=n_channels-1; channel >= 0; channel--) {
        MuFloat actual = buffer_(frame, channel);
        ASSERT_DOUBLE_EQ(expected, actual) << 
          "at (frame, channel)[tick]=(" << frame << 
          ", " << channel << 
          ")[" << tick << "]";
      }
    }
  }

  IdentityStream *identity_stream_;
  LoopStream *loop_stream_;
};

TEST_F(LoopStreamTest, Test01) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = 1;
  MuTick buffer_start = 0;
  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test02) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test03) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test04) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test05) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test06) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

// Test source stream with overlap.
//
TEST_F(LoopStreamTest, Test07) {
  MuTick interval = buffer_.frames();
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test08) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test09) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test10) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test11) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_start = 0;
  MuTick source_end = interval + 1;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_start, source_end, interval, buffer_start);
  Verify(source_start, source_end, interval, buffer_start);
}

