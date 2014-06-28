/*
 * Unit Test LoopSP
 */
#include "assert.h"
#include "crop_sp.h"
#include "identity_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define LOOP_DURATION_LT (FRAME_COUNT-5)
#define LOOP_DURATION_EQ (FRAME_COUNT)
#define LOOP_DURATION_GT (FRAME_COUNT+5)

int main() {
  mu::LoopSP loop_sp;
  mu::IdentitySP identity_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  loop_sp.setSource(NULL).setLoopDuration(LOOP_DURATION_EQ);
  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);

  loop_sp.setSource(&identity_sp).setLoopDuration(LOOP_DURATION_EQ);
  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  loop_sp.setSource(&identity_sp).setLoopDuration(LOOP_DURATION_LT);
  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(LOOP_DURATION_LT-1,0) == LOOP_DURATION_LT-1);
  ASSERT(buffer(LOOP_DURATION_LT-1,1) == LOOP_DURATION_LT-1);
  ASSERT(buffer(LOOP_DURATION_LT,0) == 0);
  ASSERT(buffer(LOOP_DURATION_LT,1) == 0);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == FRAME_COUNT-LOOP_DURATION_LT);
  ASSERT(buffer(0,1) == FRAME_COUNT-LOOP_DURATION_LT);
  
  loop_sp.setSource(&identity_sp).setLoopDuration(LOOP_DURATION_GT);
  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == FRAME_COUNT);
  ASSERT(buffer(0,1) == FRAME_COUNT);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT-1,0) == LOOP_DURATION_GT-1);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT-1,1) == LOOP_DURATION_GT-1);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT,0) == 0);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT,1) == 0);

  // regression test!
  mu::CropSP crop_sp;
  crop_sp.setSource(&identity_sp).setStart(30).setEnd(40);
  ASSERT(crop_sp.getStart() == 30);
  ASSERT(crop_sp.getEnd() == 40);
  loop_sp.setSource(&crop_sp);
  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  // adding setStart() and setEnd() to LoopSP()
  printf("=== testing setStart() and setEnd()\n");
  loop_sp.setSource(&identity_sp).setLoopDuration(FRAME_COUNT).
    setStart(mu::kIndefinite).setEnd(mu::kIndefinite);
  // resulting waveform is a "sawtooth", 0 at every multiple of
  // FRAME_COUNT

  ASSERT(loop_sp.getStart() == mu::kIndefinite);
  ASSERT(loop_sp.getEnd() == mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);

  loop_sp.step(buffer, -FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);

  loop_sp.setSource(&identity_sp).setLoopDuration(FRAME_COUNT).
    setStart(100).setEnd(mu::kIndefinite);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(99,0) == 0);
  ASSERT(buffer(100,0) == 100);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);

  loop_sp.step(buffer, -FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);

  loop_sp.setSource(&identity_sp).setLoopDuration(FRAME_COUNT).
    setStart(100).setEnd(200);

  loop_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(99,0) == 0);
  ASSERT(buffer(100,0) == 100);
  ASSERT(buffer(199,0) == 199);
  ASSERT(buffer(200,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);

  loop_sp.step(buffer, -FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);

  loop_sp.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);
  
}
