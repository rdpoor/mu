/*
 * Unit Test DelayStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define DELAY_P_100 (100)
#define DELAY_M_100 (-100)

int main() {
  mu::CropStream crop_stream;
  mu::DelayStream delay_stream;
  mu::IdentityStream identity_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  delay_stream.setSource(NULL).setDelay(DELAY_P_100);
  delay_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);

  ASSERT(delay_stream.getStart() == mu::kIndefinite);
  ASSERT(delay_stream.getEnd() == mu::kIndefinite);

  delay_stream.setSource(&identity_stream).setDelay(DELAY_P_100);
  delay_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == -DELAY_P_100);
  ASSERT(buffer(0,1) == -DELAY_P_100);
  ASSERT(buffer(511,0) == (511-DELAY_P_100));
  ASSERT(buffer(511,1) == (511-DELAY_P_100));

  ASSERT(delay_stream.getStart() == mu::kIndefinite);
  ASSERT(delay_stream.getEnd() == mu::kIndefinite);

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

  fprintf(stderr, "=== finite stream\n");
  crop_stream.setSource(&identity_stream).setStart(10).setEnd(90);
  delay_stream.setSource(&crop_stream).setDelay(0);

  ASSERT(delay_stream.getStart() == 10);
  ASSERT(delay_stream.getEnd() == 90);

  delay_stream.setDelay(20);

  ASSERT(delay_stream.getStart() == 10+20);
  ASSERT(delay_stream.getEnd() == 90+20);

  delay_stream.setDelay(-20);

  ASSERT(delay_stream.getStart() == 10-20);
  ASSERT(delay_stream.getEnd() == 90-20);

  return 0;

}
