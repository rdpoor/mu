#include "gtest/gtest.h"
#include "identity_stream.h"
#include "buffer_fixture.h"

class IdentityStream : public BufferFixture {
protected:
  void RunTest(mu::MuTick buffer_start) {
    Render(buffer_start);
    Verify(buffer_start);
  }

  void Render(mu::MuTick buffer_start) {
    identity_stream_.render(buffer_start, &buffer_);
  }
    
  void Verify(mu::MuTick buffer_start) {
    int n_frames = buffer_.frames();
    int n_channels = buffer_.channels();

    for (int frame=n_frames-1; frame >= 0; frame--) {
      mu::MuFloat expected = frame + buffer_start;
      for (int channel=n_channels-1; channel >= 0; channel--) {
        mu::MuFloat value = buffer_(frame, channel);
        ASSERT_EQ(expected, value) << 
          "at (frame, channel)[tick]=(" << frame << 
          ", " << channel << 
          ")[" << frame + buffer_start << "]";
      }
    }
  }

  mu::IdentityStream identity_stream_;
};

TEST_F(IdentityStream, Render) {
  mu::MuTick buffer_start = 0;
  RunTest(buffer_start);
}

TEST_F(IdentityStream, RenderOffset) {
  mu::MuTick buffer_start = -22;
  RunTest(buffer_start);
}


