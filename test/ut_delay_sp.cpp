/*
 * Unit Test DelaySP
 */
#include "assert.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_sp.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define DELAY_P_100 (100)
#define DELAY_M_100 (-100)

int main() {
  mu::CropSP crop_sp;
  mu::DelaySP delay_sp;
  mu::IdentitySP identity_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  delay_sp.setSource(NULL).setDelay(DELAY_P_100);
  delay_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0);

  ASSERT(delay_sp.getStart() == mu::kIndefinite);
  ASSERT(delay_sp.getEnd() == mu::kIndefinite);

  delay_sp.setSource(&identity_sp).setDelay(DELAY_P_100);
  delay_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == -DELAY_P_100);
  ASSERT(buffer(0,1) == -DELAY_P_100);
  ASSERT(buffer(511,0) == (511-DELAY_P_100));
  ASSERT(buffer(511,1) == (511-DELAY_P_100));

  ASSERT(delay_sp.getStart() == mu::kIndefinite);
  ASSERT(delay_sp.getEnd() == mu::kIndefinite);

  delay_sp.setSource(&identity_sp).setDelay(DELAY_P_100);
  delay_sp.step(buffer, DELAY_P_100, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(511,0) == 511);
  ASSERT(buffer(511,1) == 511);

  delay_sp.setSource(&identity_sp).setDelay(DELAY_M_100);
  delay_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == -DELAY_M_100);
  ASSERT(buffer(0,1) == -DELAY_M_100);
  ASSERT(buffer(511,0) == (511-DELAY_M_100));
  ASSERT(buffer(511,1) == (511-DELAY_M_100));

  delay_sp.setSource(&identity_sp).setDelay(DELAY_M_100);
  delay_sp.step(buffer, DELAY_M_100, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(511,0) == 511);
  ASSERT(buffer(511,1) == 511);

  fprintf(stderr, "=== finite stream\n");
  crop_sp.setSource(&identity_sp).setStart(10).setEnd(90);
  delay_sp.setSource(&crop_sp).setDelay(0);

  ASSERT(delay_sp.getStart() == 10);
  ASSERT(delay_sp.getEnd() == 90);

  delay_sp.setDelay(20);

  ASSERT(delay_sp.getStart() == 10+20);
  ASSERT(delay_sp.getEnd() == 90+20);

  delay_sp.setDelay(-20);

  ASSERT(delay_sp.getStart() == 10-20);
  ASSERT(delay_sp.getEnd() == 90-20);

  return 0;

}
