#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "mock_mu_stream.h"
#include "mock_player.h"
#include "transport.h"

using ::testing::_;

class TransportTest : public testing::Test {
};

const int kFrameCount = 512;

TEST(TransportTest, Render) {
  MockMuStream mu_stream;
  MockPlayer player;
  stk::StkFrames frames;
  mu::Transport transport;

  EXPECT_CALL(player, start());
  EXPECT_CALL(mu_stream, render(0, _));

  frames.resize(kFrameCount, 2);
  transport.set_source(&mu_stream);
  transport.set_player(&player);
  transport.run();
  transport.render(&frames);
} 
