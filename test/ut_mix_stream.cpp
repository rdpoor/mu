/*
 * Unit Test MixStream
 */
#include "assert.h"
#include "crop_stream.h"
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
  mu::CropStream crop_stream_a;
  mu::CropStream crop_stream_b;
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

  fprintf(stderr,"=== getStart() tests\n");
  crop_stream_a.setSource(&identity_stream_a).setStart(mu::kIndefinite).setEnd(mu::kIndefinite);
  crop_stream_b.setSource(&identity_stream_b).setStart(mu::kIndefinite).setEnd(mu::kIndefinite);

  fprintf(stderr, "=== null sources\n");
  mix_stream.setSourceA(NULL).setSourceB(NULL);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  fprintf(stderr, "=== non-null A source, null B source\n");
  mix_stream.setSourceA(&crop_stream_a).setSourceB(NULL);
  crop_stream_a.setStart(mu::kIndefinite);
  crop_stream_b.setStart(mu::kIndefinite);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  crop_stream_a.setStart(10);
  ASSERT(mix_stream.getStart() == 10);

  fprintf(stderr, "=== null A source, non-null B source\n");
  mix_stream.setSourceA(NULL).setSourceB(&crop_stream_b);
  crop_stream_a.setStart(mu::kIndefinite);
  crop_stream_b.setStart(mu::kIndefinite);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  crop_stream_b.setStart(20);
  ASSERT(mix_stream.getStart() == 20);

  fprintf(stderr, "=== non-null A source, non-null B source\n");
  mix_stream.setSourceA(&crop_stream_a).setSourceB(&crop_stream_b);
  crop_stream_a.setStart(mu::kIndefinite);
  crop_stream_b.setStart(mu::kIndefinite);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  crop_stream_a.setStart(10);
  crop_stream_b.setStart(mu::kIndefinite);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  crop_stream_a.setStart(mu::kIndefinite);
  crop_stream_b.setStart(20);
  ASSERT(mix_stream.getStart() == mu::kIndefinite);

  crop_stream_a.setStart(10);
  crop_stream_b.setStart(20);
  ASSERT(mix_stream.getStart() == 10);

  fprintf(stderr,"=== getEnd() tests\n");
  crop_stream_a.setSource(&identity_stream_a).setStart(mu::kIndefinite).setEnd(mu::kIndefinite);
  crop_stream_b.setSource(&identity_stream_b).setStart(mu::kIndefinite).setEnd(mu::kIndefinite);

  fprintf(stderr, "=== null sources\n");
  mix_stream.setSourceA(NULL).setSourceB(NULL);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr, "=== non-null A source, null B source\n");
  mix_stream.setSourceA(&crop_stream_a).setSourceB(NULL);
  crop_stream_a.setEnd(mu::kIndefinite);
  crop_stream_b.setEnd(mu::kIndefinite);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  crop_stream_a.setEnd(10);
  ASSERT(mix_stream.getEnd() == 10);

  fprintf(stderr, "=== null A source, non-null B source\n");
  mix_stream.setSourceA(NULL).setSourceB(&crop_stream_b);
  crop_stream_a.setEnd(mu::kIndefinite);
  crop_stream_b.setEnd(mu::kIndefinite);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  crop_stream_b.setEnd(20);
  ASSERT(mix_stream.getEnd() == 20);

  fprintf(stderr, "=== non-null A source, non-null B source\n");
  mix_stream.setSourceA(&crop_stream_a).setSourceB(&crop_stream_b);
  crop_stream_a.setEnd(mu::kIndefinite);
  crop_stream_b.setEnd(mu::kIndefinite);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  crop_stream_a.setEnd(10);
  crop_stream_b.setEnd(mu::kIndefinite);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  crop_stream_a.setEnd(mu::kIndefinite);
  crop_stream_b.setEnd(20);
  ASSERT(mix_stream.getEnd() == mu::kIndefinite);

  crop_stream_a.setEnd(10);
  crop_stream_b.setEnd(20);
  ASSERT(mix_stream.getEnd() == 20);

  fprintf(stderr,"=== done\n");
  return 0;

}
