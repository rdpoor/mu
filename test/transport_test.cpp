#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mock_render_stream.h"
#include "mock_player.h"
#include "transport.h"

using ::testing::_;

class TransportTest : public testing::Test {
};

const int kFrameCount = 512;

TEST(TransportTest, Render) {
  MockRenderStream render_stream;
  MockPlayer player;
  stk::StkFrames frames;
  mu::Transport transport;

  EXPECT_CALL(player, start());
  EXPECT_CALL(render_stream, render(_, 0, 0, kFrameCount));

  frames.resize(kFrameCount, 2);
  transport.set_source(&render_stream);
  transport.set_player(&player);
  transport.run();
  transport.render(frames);
} 
