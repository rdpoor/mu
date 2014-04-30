/*
 * Unit Test IdentityStream
 */
#include "assert.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define OFFSET_P (100)
#define OFFSET_N (-100)

int main() {
  mu::IdentityStream identity_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  identity_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  identity_stream.step(buffer, OFFSET_P, player);

  ASSERT(buffer(0,0) == OFFSET_P);
  ASSERT(buffer(0,1) == OFFSET_P);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1)+OFFSET_P);
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1)+OFFSET_P);

  identity_stream.step(buffer, OFFSET_N, player);

  ASSERT(buffer(0,0) == OFFSET_N);
  ASSERT(buffer(0,1) == OFFSET_N);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1)+OFFSET_N);
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1)+OFFSET_N);

  return 0;

}
