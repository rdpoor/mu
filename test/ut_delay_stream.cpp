/*
 * Unit Test DelayStream
 */
#include "assert.h"
#include "delay_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define BUFFER_FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define DELAY_P_100 (100)
#define DELAY_M_100 (-100)

int main() {
  mu::IdentityStream identity_stream;
  mu::DelayStream delay_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  // TODO: make constants for these
  buffer.setDataRate(FRAME_RATE);
  buffer.resize(BUFFER_FRAME_COUNT, CHANNEL_COUNT);

  delay_stream.setSource(NULL).setDelay(DELAY_P_100);
  delay_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);

  delay_stream.setSource(&identity_stream).setDelay(DELAY_P_100);
  delay_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == -DELAY_P_100);
  ASSERT(buffer(0,1) == -DELAY_P_100);
  ASSERT(buffer(511,0) == (511-DELAY_P_100));
  ASSERT(buffer(511,1) == (511-DELAY_P_100));

  delay_stream.setSource(&identity_stream).setDelay(DELAY_P_100);
  delay_stream.step(buffer, DELAY_P_100, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(511,0) == 511);
  ASSERT(buffer(511,1) == 511);

  delay_stream.setSource(&identity_stream).setDelay(DELAY_M_100);
  delay_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == -DELAY_M_100);
  ASSERT(buffer(0,1) == -DELAY_M_100);
  ASSERT(buffer(511,0) == (511-DELAY_M_100));
  ASSERT(buffer(511,1) == (511-DELAY_M_100));

  delay_stream.setSource(&identity_stream).setDelay(DELAY_M_100);
  delay_stream.step(buffer, DELAY_M_100, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(511,0) == 511);
  ASSERT(buffer(511,1) == 511);

  return 0;

}
