// loop(t, source, source_end, interval) =
//   sum[i=0,inf] source(t - i * interval), 0 <= (t - i * interval) < source_end
// imax:
//   0 <= t - imax * interval
//   imax * interval <= t
//   imax <= t / interval
// imin:
//   t - imin * interval < source_end
//   t - source_end < imin * interval
//   imin > (t - source_end) / interval
//
// ilo = floor((t - source_end) / interval) + 1
// ihi = floor(t / interval)


#include "mu_utils.h"
#include "gtest/gtest.h"
#include "loop_stream.h"
#include "identity_stream.h"
#include "buffer_fixture.h"
#include <cmath>

using namespace mu;

class LoopStreamTest : public BufferFixture {
protected:
  void SetUp() {
    BufferFixture::SetUp();
    MuUtils::zero_buffer(&buffer_);
    identity_stream_ = new mu::IdentityStream();
    loop_stream_ = new mu::LoopStream();
  }

  MuFloat Basis(MuTick tick) {
    return (MuFloat)tick;
  }

  void RunTest(MuTick source_end, 
               MuTick interval, 
               MuTick buffer_start) {
    loop_stream_->set_source(identity_stream_);
    loop_stream_->set_source_end(source_end);
    loop_stream_->set_interval(interval);

    loop_stream_->render(buffer_start, &buffer_);
  }

  MuFloat Expect(MuTick source_end, 
                 MuTick interval, 
                 MuTick tick) {
    int ilo = floor((tick - source_end) / (double)interval) + 1;
    int ihi = floor(tick / (double)interval);
    // comment out next line for infinite extent
    ilo = std::max(ilo, 0);

    MuFloat sum = 0.0;
    for (int i=ilo; i<=ihi; ++i) {
      sum += Basis(tick - i * interval);
    }
    return sum;
  }

  void Verify(MuTick source_end, 
              MuTick interval, 
              MuTick buffer_start) {
    int n_frames = buffer_.frames();
    int n_channels = buffer_.channels();

    for (int frame=n_frames-1; frame >= 0; frame--) {
      MuTick tick = frame + buffer_start;
      MuFloat expected = Expect(source_end, interval, tick);
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
  MuTick source_end = 1;
  MuTick buffer_start = 0;
  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test02) {
  MuTick interval = buffer_.frames();
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test03) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test04) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_end = interval;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test05) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_end = interval;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test06) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_end = interval;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

// Test source stream with overlap.
//
TEST_F(LoopStreamTest, Test07) {
  MuTick interval = buffer_.frames();
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test08) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test09) {
  MuTick interval = buffer_.frames() - 1;
  MuTick source_end = interval + 1;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test10) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_end = interval + 1;
  MuTick buffer_start = 0;

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

TEST_F(LoopStreamTest, Test11) {
  MuTick interval = buffer_.frames() + 1;
  MuTick source_end = interval + 1;
  MuTick buffer_start = buffer_.frames();

  RunTest(source_end, interval, buffer_start);
  Verify(source_end, interval, buffer_start);
}

