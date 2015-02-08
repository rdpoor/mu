#include "gtest/gtest.h"
#include "piecewise_linear_stream.h"
#include "buffer_fixture.h"

class PiecewiseLinearStream : public BufferFixture {
protected:
  mu::PiecewiseLinearStream *piecewise_linear_stream_;
};

TEST_F(PiecewiseLinearStream, NoBreakpoints) {
  mu::MuTick buffer_start = 0;
  piecewise_linear_stream_ = new mu::PiecewiseLinearStream();

  ASSERT_FALSE(piecewise_linear_stream_->render(buffer_start, &buffer_));
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  for (int frame = n_frames-1; frame >= 0; --frame) {
    mu::MuFloat expected = BufferFixture::guard_value();
    for (int channel = n_channels-1; channel >= 0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_EQ(expected, actual) << 
        "at (frame, channel)[tick]=(" << frame << "," << channel << ")[" << (frame + buffer_start) << "]";
    }
  }
}

TEST_F(PiecewiseLinearStream, OneBreakpoint) {
  mu::MuTick buffer_start = 0;
  mu::MuFloat value = 33.2;
  piecewise_linear_stream_ = new mu::PiecewiseLinearStream();
  piecewise_linear_stream_->add_breakpoint(buffer_.frames()/2, value);

  ASSERT_TRUE(piecewise_linear_stream_->render(buffer_start, &buffer_));
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  for (int frame = n_frames-1; frame >= 0; --frame) {
    mu::MuFloat expected = value;
    for (int channel = n_channels-1; channel >= 0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_EQ(expected, actual) << 
        "at (frame, channel)[tick]=(" << frame << "," << channel << ")[" << (frame + buffer_start) << "]";
    }
  }
}

TEST_F(PiecewiseLinearStream, TwoBreakpoints) {
  mu::MuTick buffer_start = 0;
  mu::MuTick x0 = 10;
  mu::MuFloat y0 = 200.0;
  mu::MuTick x1 = 20;
  mu::MuFloat y1 = 100.0;

  piecewise_linear_stream_ = new mu::PiecewiseLinearStream();
  piecewise_linear_stream_->add_breakpoint(x0, y0);
  piecewise_linear_stream_->add_breakpoint(x1, y1);

  ASSERT_TRUE(piecewise_linear_stream_->render(buffer_start, &buffer_));

  int n_channels = buffer_.channels();
  for (int channel = n_channels-1; channel >= 0; --channel) {
    ASSERT_DOUBLE_EQ(y0, buffer_(buffer_start + x0 - 1, channel));
    ASSERT_DOUBLE_EQ(y0, buffer_(buffer_start + x0, channel));
    ASSERT_DOUBLE_EQ((y0 + y1) / 2.0, buffer_(buffer_start + (x0 + x1) / 2.0, channel));
    ASSERT_DOUBLE_EQ(y1, buffer_(buffer_start + x1, channel));
    ASSERT_DOUBLE_EQ(y1, buffer_(buffer_start + x1 + 1, channel));
  }
}

TEST_F(PiecewiseLinearStream, ThreeBreakpoints) {
  mu::MuTick buffer_start = 0;
  mu::MuTick x0 = 0;
  mu::MuFloat y0 = 100.0;
  mu::MuTick x1 = buffer_.frames() / 2.0;
  mu::MuFloat y1 = 200.0;
  mu::MuTick x2 = buffer_.frames();
  mu::MuFloat y2 = 0.0;

  piecewise_linear_stream_ = new mu::PiecewiseLinearStream();
  piecewise_linear_stream_->add_breakpoint(x0, y0);
  piecewise_linear_stream_->add_breakpoint(x1, y1);
  piecewise_linear_stream_->add_breakpoint(x2, y2);

  ASSERT_TRUE(piecewise_linear_stream_->render(buffer_start, &buffer_));

#if 0
  int n_frames = buffer_.frames();
  for (int frame = 0; frame < n_frames; ++frame) {
    printf("%2d %lf\n", frame, buffer_(frame, 0));
  }
#endif

  int n_channels = buffer_.channels();
  for (int channel = n_channels-1; channel >= 0; --channel) {
    ASSERT_DOUBLE_EQ(y0, buffer_(buffer_start + x0, channel));
    ASSERT_DOUBLE_EQ(y0 + (y1 - y0) / (x1 - x0), buffer_(buffer_start + x0 + 1, channel));
    ASSERT_DOUBLE_EQ(y1, buffer_(buffer_start + x1, channel));
    ASSERT_DOUBLE_EQ(y1 + (x2 - 1 - x1) * (y2 - y1) / (x2 - x1), buffer_(buffer_start + x2 - 1, channel));
  }
}



