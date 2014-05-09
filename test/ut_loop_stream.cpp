/*
 * Unit Test LoopStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "identity_stream.h"
#include "loop_stream.h"
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
  mu::LoopStream loop_stream;
  mu::IdentityStream identity_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  // fprintf(stderr,"A");
  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  // fprintf(stderr,"B");
  loop_stream.setSource(NULL).setLoopDuration(LOOP_DURATION_EQ);
  ASSERT(loop_stream.getStart() == mu::kIndefinite);
  ASSERT(loop_stream.getEnd() == mu::kIndefinite);

  // fprintf(stderr,"C");
  loop_stream.step(buffer, 0, player);
  // fprintf(stderr,"C1");
  ASSERT(buffer(0,0) == 0);

  // fprintf(stderr,"D");
  loop_stream.setSource(&identity_stream).setLoopDuration(LOOP_DURATION_EQ);
  ASSERT(loop_stream.getStart() == mu::kIndefinite);
  ASSERT(loop_stream.getEnd() == mu::kIndefinite);

  // fprintf(stderr,"E");
  loop_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  // fprintf(stderr,"F");
  loop_stream.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  // fprintf(stderr,"G");
  loop_stream.setSource(&identity_stream).setLoopDuration(LOOP_DURATION_LT);
  ASSERT(loop_stream.getStart() == mu::kIndefinite);
  ASSERT(loop_stream.getEnd() == mu::kIndefinite);

  // fprintf(stderr,"H");
  loop_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(LOOP_DURATION_LT-1,0) == LOOP_DURATION_LT-1);
  ASSERT(buffer(LOOP_DURATION_LT-1,1) == LOOP_DURATION_LT-1);
  ASSERT(buffer(LOOP_DURATION_LT,0) == 0);
  ASSERT(buffer(LOOP_DURATION_LT,1) == 0);

  loop_stream.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == FRAME_COUNT-LOOP_DURATION_LT);
  ASSERT(buffer(0,1) == FRAME_COUNT-LOOP_DURATION_LT);
  
  loop_stream.setSource(&identity_stream).setLoopDuration(LOOP_DURATION_GT);
  ASSERT(loop_stream.getStart() == mu::kIndefinite);
  ASSERT(loop_stream.getEnd() == mu::kIndefinite);

  loop_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  loop_stream.step(buffer, FRAME_COUNT, player);
  ASSERT(buffer(0,0) == FRAME_COUNT);
  ASSERT(buffer(0,1) == FRAME_COUNT);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT-1,0) == LOOP_DURATION_GT-1);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT-1,1) == LOOP_DURATION_GT-1);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT,0) == 0);
  ASSERT(buffer(LOOP_DURATION_GT-FRAME_COUNT,1) == 0);

  // regression test!
  mu::CropStream crop_stream;
  crop_stream.setSource(&identity_stream).setStart(30).setEnd(40);
  ASSERT(crop_stream.getStart() == 30);
  ASSERT(crop_stream.getEnd() == 40);
  loop_stream.setSource(&crop_stream);
  ASSERT(loop_stream.getStart() == mu::kIndefinite);
  ASSERT(loop_stream.getEnd() == mu::kIndefinite);
  
  
}
