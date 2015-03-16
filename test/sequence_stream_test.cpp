#include "mu_utils.h"
#include "gtest/gtest.h"
#include "dirac_stream.h"
#include "sequence_stream.h"
#include "buffer_fixture.h"

using namespace mu;

class SequenceStreamTest : public BufferFixture {
protected:
  void SetUp() {
    BufferFixture::SetUp();
    MuUtils::zero_buffer(&buffer_);
    dirac_stream_ = new mu::DiracStream();
    sequence_stream_ = new mu::SequenceStream();
  }

  void RunTest(mu::MuTick buffer_start) {
    sequence_stream_->render(buffer_start, &buffer_);
  }

  DiracStream *dirac_stream_;
  SequenceStream *sequence_stream_;
};


TEST_F(SequenceStreamTest, Test01) {
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();

  mu::MuTick buffer_start = 0;
  mu::MuTick buffer_end = buffer_start + n_frames;

  RunTest(buffer_start);
  for (mu::MuTick frame = buffer_end-1; frame >= 0; --frame) {
    mu::MuFloat expected = 0.0;
    for (int channel = n_channels-1; channel >=0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(SequenceStreamTest, Test02) {
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  mu::MuTick buffer_start = 0;
  mu::MuTick buffer_end = buffer_start + n_frames;

  sequence_stream_->add_source(dirac_stream_, buffer_start, 1.0);

  RunTest(buffer_start);
  for (mu::MuTick frame = buffer_end-1; frame >= 0; --frame) {
    mu::MuFloat expected = (frame == buffer_start) ? 1.0 : 0.0;
    for (int channel = n_channels-1; channel >=0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(SequenceStreamTest, Test03) {
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  mu::MuTick buffer_start = 0;
  mu::MuTick buffer_end = buffer_start + n_frames;

  sequence_stream_->add_source(dirac_stream_, buffer_start+1, 1.0);
  sequence_stream_->add_source(dirac_stream_, buffer_start+3, 1.0);

  RunTest(buffer_start);
  for (mu::MuTick frame = buffer_end-1; frame >= 0; --frame) {
    mu::MuFloat expected = 0.0;
    if (frame == buffer_start + 1) {
      expected = 1.0;
    } else if (frame == buffer_start + 3) {
      expected = 1.0;
    }
    for (int channel = n_channels-1; channel >=0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(SequenceStreamTest, Test04) {
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  mu::MuTick buffer_start = 0;
  mu::MuTick buffer_end = buffer_start + n_frames;

  sequence_stream_->add_source(dirac_stream_, buffer_start+1, 1.0);
  sequence_stream_->add_source(dirac_stream_, buffer_start+3, 2.0);

  RunTest(buffer_start);
  for (mu::MuTick frame = buffer_end-1; frame >= 0; --frame) {
    mu::MuFloat expected = 0.0;
    if (frame == buffer_start + 1) {
      expected = 1.0;
    } else if (frame == buffer_start + 3) {
      expected = 2.0;
    }
    for (int channel = n_channels-1; channel >=0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(SequenceStreamTest, Test05) {
  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  mu::MuTick buffer_start = 0;
  mu::MuTick buffer_end = buffer_start + n_frames;

  sequence_stream_->add_source(dirac_stream_, buffer_start+4, 1.0);
  sequence_stream_->add_source(dirac_stream_, buffer_start+4, 2.0);

  RunTest(buffer_start);
  for (mu::MuTick frame = buffer_end-1; frame >= 0; --frame) {
    mu::MuFloat expected = 0.0;
    if (frame == buffer_start + 4) {
      expected = 3.0;
    }
    for (int channel = n_channels-1; channel >=0; --channel) {
      mu::MuFloat actual = buffer_(frame, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

