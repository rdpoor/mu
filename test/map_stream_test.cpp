#include "gtest/gtest.h"
#include "map_stream.h"
#include "mu_types.h"
#include "identity_stream.h"
#include "buffer_fixture.h"

class MapStream : public BufferFixture {
protected:
  mu::MapStream *map_stream_;
  mu::IdentityStream *identity_stream_;
};

mu::MuFloat invert(mu::MuFloat sample, mu::MuTick tick) {
  (void)tick;
  return -sample;
}

TEST_F(MapStream, NoInput) {
  mu::MuTick buffer_start = 0;
  map_stream_ = new mu::MapStream();
  map_stream_->set_callback(invert);
  ASSERT_FALSE(map_stream_->render(buffer_start, &buffer_));
}

TEST_F(MapStream, NoCallback) {
  mu::MuTick buffer_start = 0;
  map_stream_ = new mu::MapStream();
  identity_stream_ = new mu::IdentityStream();
  map_stream_->set_source(identity_stream_);
  ASSERT_FALSE(map_stream_->render(buffer_start, &buffer_));
}

TEST_F(MapStream, BasicTest) {
  mu::MuTick buffer_start = 0;
  map_stream_ = new mu::MapStream();
  identity_stream_ = new mu::IdentityStream();
  map_stream_->set_source(identity_stream_);
  map_stream_->set_callback(invert);
  ASSERT_TRUE(map_stream_->render(buffer_start, &buffer_));

  int n_frames = buffer_.frames();
  int n_channels = buffer_.channels();
  
  for (int frame=n_frames-1; frame >= 0; frame--) {
    mu::MuTick tick = frame + buffer_start;
    mu::MuFloat expected = -tick;
    for (int channel=n_channels-1; channel >= 0; channel--) {
      mu::MuFloat value = buffer_(frame, channel);
      ASSERT_EQ(expected, value) << 
        "at (frame, channel)[tick]=(" << frame << 
        ", " << channel << 
        ")[" << frame + buffer_start << "]";
    }
  }
}
