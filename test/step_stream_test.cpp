#include "gtest/gtest.h"
#include "step_stream.h"
#include "buffer_fixture.h"

class StepStream : public BufferFixture {
protected:
  mu::StepStream *step_stream_;
};

TEST_F(StepStream, NoBreakpoints) {
  mu::MuTick buffer_start = 0;
  step_stream_ = new mu::StepStream();

  ASSERT_FALSE(step_stream_->render(buffer_start, &buffer_));
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

TEST_F(StepStream, OneBreakpoint) {
  mu::MuTick buffer_start = 0;
  mu::MuFloat value = 33.2;
  step_stream_ = new mu::StepStream();
  step_stream_->add_breakpoint(buffer_.frames()/2, value);

  ASSERT_TRUE(step_stream_->render(buffer_start, &buffer_));
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

TEST_F(StepStream, TwoBreakpoints) {
  mu::MuTick buffer_start = 0;
  mu::MuTick x0 = 10;
  mu::MuFloat y0 = 200.0;
  mu::MuTick x1 = 20;
  mu::MuFloat y1 = 100.0;

  step_stream_ = new mu::StepStream();
  step_stream_->add_breakpoint(x0, y0);
  step_stream_->add_breakpoint(x1, y1);

  ASSERT_TRUE(step_stream_->render(buffer_start, &buffer_));

  int n_channels = buffer_.channels();
  for (int channel = n_channels-1; channel >= 0; --channel) {
    ASSERT_DOUBLE_EQ(buffer_(buffer_start + x0 - 1, channel), y0);
    ASSERT_DOUBLE_EQ(buffer_(buffer_start + x0, channel), y0);
    ASSERT_DOUBLE_EQ(buffer_(buffer_start + (x0 + x1) / 2.0, channel), y0);
    ASSERT_DOUBLE_EQ(buffer_(buffer_start + x1, channel), y1);
    ASSERT_DOUBLE_EQ(buffer_(buffer_start + x1 + 1, channel), y1);
  }
}

TEST_F(StepStream, ThreeBreakpoints) {
  mu::MuTick buffer_start = 0;
  mu::MuTick x0 = 0;
  mu::MuFloat y0 = 100.0;
  mu::MuTick x1 = buffer_.frames() / 2.0;
  mu::MuFloat y1 = 200.0;
  mu::MuTick x2 = buffer_.frames() - 1;
  mu::MuFloat y2 = 0.0;

  step_stream_ = new mu::StepStream();
  step_stream_->add_breakpoint(x0, y0);
  step_stream_->add_breakpoint(x1, y1);
  step_stream_->add_breakpoint(x2, y2);

  printf("%s\n", step_stream_->inspect().c_str());

  ASSERT_TRUE(step_stream_->render(buffer_start, &buffer_));

#if 0
  int n_frames = buffer_.frames();
  for (int frame = 0; frame < n_frames; ++frame) {
    printf("%2d %lf\n", frame, buffer_(frame, 0));
  }
#endif

  int n_channels = buffer_.channels();
  for (int channel = n_channels-1; channel >= 0; --channel) {
    ASSERT_DOUBLE_EQ(y0, buffer_(buffer_start + x0, channel));
    ASSERT_DOUBLE_EQ(y0, buffer_(buffer_start + x1 - 1, channel));
    ASSERT_DOUBLE_EQ(y1, buffer_(buffer_start + x1, channel));
    ASSERT_DOUBLE_EQ(y1, buffer_(buffer_start + x2 - 1, channel));
    ASSERT_DOUBLE_EQ(y2, buffer_(buffer_start + x2, channel));
  }
}

