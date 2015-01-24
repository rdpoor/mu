#include "gtest/gtest.h"
#include "delay_stream.h"
#include "dirac_stream.h"
#include "buffer_fixture.h"

class DelayStream : public BufferFixture {
protected:
  void RunTest(mu::MuTick buffer_start, mu::MuTick delay) {
    delay_stream_.set_delay(delay);
    delay_stream_.set_source(&dirac_stream_);
    Render(buffer_start);
    Verify(buffer_start, delay);
  }

  void Render(mu::MuTick buffer_start) {
    delay_stream_.render(buffer_start, &buffer_);
  }
    
  void Verify(mu::MuTick buffer_start, mu::MuTick delay) {
    int n_frames = buffer_.frames();
    int n_channels = buffer_.channels();

    for (int frame=n_frames-1; frame >= 0; frame--) {
      mu::MuTick tick = frame + buffer_start;
      mu::MuFloat expected = (tick == delay) ? 1.0 : 0.0;
      for (int channel=n_channels-1; channel >= 0; channel--) {
        mu::MuFloat value = buffer_(frame, channel);
        ASSERT_EQ(expected, value) << 
          "at (frame, channel)[tick]=(" << frame << 
          ", " << channel << 
          ")[" << frame + buffer_start << "]";
      }
    }
  }

  mu::DelayStream delay_stream_;
  mu::DiracStream dirac_stream_;
};

TEST_F(DelayStream, S0D0) {
  mu::MuTick buffer_start = 0;
  mu::MuTick delay = 0;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, S0Dp) {
  mu::MuTick buffer_start = 0;
  mu::MuTick delay = 10;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, S0Dn) {
  mu::MuTick buffer_start = 0;
  mu::MuTick delay = -10;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SpD0) {
  mu::MuTick buffer_start = 10;
  mu::MuTick delay = 0;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SpDp) {
  mu::MuTick buffer_start = 10;
  mu::MuTick delay = 10;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SpDn) {
  mu::MuTick buffer_start = 10;
  mu::MuTick delay = -10;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SnD0) {
  mu::MuTick buffer_start = -10;
  mu::MuTick delay = 0;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SnDp) {
  mu::MuTick buffer_start = -10;
  mu::MuTick delay = 10;
  RunTest(buffer_start, delay);
}

TEST_F(DelayStream, SnDn) {
  mu::MuTick buffer_start = -10;
  mu::MuTick delay = -10;
  RunTest(buffer_start, delay);
}

