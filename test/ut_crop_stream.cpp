/*
 * Unit Test CropStream
 */
#include "assert.h"
#include "crop_stream.h"
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
  mu::IdentityStream identity_stream;
  mu::CropStream crop_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  fprintf(stderr,"=== null input\n");
  crop_stream.setSource(NULL);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(crop_stream.getStart() == mu::kIndefinite);
  ASSERT(crop_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== default start and end\n");
  crop_stream.setSource(&identity_stream);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(crop_stream.getStart() == mu::kIndefinite);
  ASSERT(crop_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== definite start and end\n");
  crop_stream.setSource(&identity_stream).setStart(10).setEnd(90);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(9,0) == 0);
  ASSERT(buffer(9,1) == 0);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(10,1) == 10);
  ASSERT(buffer(89,0) == 89);
  ASSERT(buffer(89,1) == 89);
  ASSERT(buffer(90,0) == 0);
  ASSERT(buffer(90,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(crop_stream.getStart() == 10);
  ASSERT(crop_stream.getEnd() == 90);

  fprintf(stderr,"=== indefinite start, definite end\n");
  crop_stream.setSource(&identity_stream).setStart(mu::kIndefinite).setEnd(90);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(9,0) == 9);
  ASSERT(buffer(9,1) == 9);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(10,1) == 10);
  ASSERT(buffer(89,0) == 89);
  ASSERT(buffer(89,1) == 89);
  ASSERT(buffer(90,0) == 0);
  ASSERT(buffer(90,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(crop_stream.getStart() == mu::kIndefinite);
  ASSERT(crop_stream.getEnd() == 90);

  fprintf(stderr,"=== definite start, indefinite end\n");
  crop_stream.setSource(&identity_stream).setStart(10).setEnd(mu::kIndefinite);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(9,0) == 0);
  ASSERT(buffer(9,1) == 0);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(10,1) == 10);
  ASSERT(buffer(89,0) == 89);
  ASSERT(buffer(89,1) == 89);
  ASSERT(buffer(90,0) == 90);
  ASSERT(buffer(90,1) == 90);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(crop_stream.getStart() == 10);
  ASSERT(crop_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== indefinite start, indefinite end\n");
  crop_stream.setSource(&identity_stream).setStart(mu::kIndefinite).setEnd(mu::kIndefinite);
  crop_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(9,0) == 9);
  ASSERT(buffer(9,1) == 9);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(10,1) == 10);
  ASSERT(buffer(89,0) == 89);
  ASSERT(buffer(89,1) == 89);
  ASSERT(buffer(90,0) == 90);
  ASSERT(buffer(90,1) == 90);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(crop_stream.getStart() == mu::kIndefinite);
  ASSERT(crop_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== done\n");
  return 0;

}
