/*
 * Unit Test MixStream
 */
#include "assert.h"
#include "mix_stream.h"
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
  mu::IdentityStream identity_stream_a;
  mu::IdentityStream identity_stream_b;
  mu::MixStream mix_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  fprintf(stderr,"=== both inputs null\n");
  mix_stream.setSourceA(NULL);
  mix_stream.setSourceB(NULL);
  mix_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(1,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  fprintf(stderr,"=== input A only\n");
  mix_stream.setSourceA(&identity_stream_a);
  mix_stream.setSourceB(NULL);
  mix_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  fprintf(stderr,"=== input B only\n");
  mix_stream.setSourceA(NULL);
  mix_stream.setSourceB(&identity_stream_b);
  mix_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  fprintf(stderr,"=== inputs A+B\n");
  mix_stream.setSourceA(&identity_stream_a);
  mix_stream.setSourceB(&identity_stream_b);
  mix_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 2);
  ASSERT(buffer(1,1) == 2);
  ASSERT(buffer((FRAME_COUNT-1),0) == 2*(FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == 2*(FRAME_COUNT-1));

  fprintf(stderr,"=== done\n");
  return 0;

}
