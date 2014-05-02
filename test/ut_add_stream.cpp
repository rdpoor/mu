/*
 * Unit Test MixStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "add_stream.h"
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
  mu::AddStream add_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  // TODO: test addSource(), removeSource(), removeAllSources()

  /*
   * configuration:
   * stream 1: start indefinite, end indefinite
   * stream 2: start frame 0, end frame 8
   * stream 3: start frame 6, end frame 14
   * stream 4: start frame 12, end frame 20
   *
   * nb: stream3 overlaps stream2 and stream4.  
   *     stream2 and stream4 are disjoint.
   */
  mu::Stream *stream1 = new mu::IdentityStream();
  mu::Stream *stream2 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(0).setEnd(8);
  mu::Stream *stream3 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(6).setEnd(14);
  mu::Stream *stream4 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(12).setEnd(20);

  fprintf(stderr, "=== assay...\n");
  fprintf(stderr, "stream1->getStart()=%ld, getEnd()=%ld\n", stream1->getStart(), stream1->getEnd());
  fprintf(stderr, "stream2->getStart()=%ld, getEnd()=%ld\n", stream2->getStart(), stream2->getEnd());
  fprintf(stderr, "stream3->getStart()=%ld, getEnd()=%ld\n", stream3->getStart(), stream3->getEnd());
  fprintf(stderr, "stream4->getStart()=%ld, getEnd()=%ld\n", stream4->getStart(), stream4->getEnd());

  fprintf(stderr, "=== no inputs\n");
  add_stream.removeAllSources();
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart()==mu::kIndefinite);
  ASSERT(add_stream.getEnd()==mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== stream1 only\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream1);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(add_stream.getStart()==mu::kIndefinite);
  ASSERT(add_stream.getEnd()==mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== stream2 only\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream2);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 0);
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 0);
  ASSERT(add_stream.getEnd() == 8);

  // ================================================================
  fprintf(stderr, "=== stream3 only\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream3);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(5,0) == 0);
  ASSERT(buffer(5,1) == 0);
  ASSERT(buffer(6,0) == 6);
  ASSERT(buffer(6,1) == 6);
  ASSERT(buffer(13,0) == 13);
  ASSERT(buffer(13,1) == 13);
  ASSERT(buffer(14,0) == 0);
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 6);
  ASSERT(add_stream.getEnd() == 14);

  // ================================================================
  fprintf(stderr, "=== stream4 only\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream4);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(11,0) == 0);
  ASSERT(buffer(11,1) == 0);
  ASSERT(buffer(12,0) == 12);
  ASSERT(buffer(12,1) == 12);
  ASSERT(buffer(19,0) == 19);
  ASSERT(buffer(19,1) == 19);
  ASSERT(buffer(20,0) == 0);
  ASSERT(buffer(20,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 12);
  ASSERT(add_stream.getEnd() == 20);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream2\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream1);
  add_stream.addSource(stream2);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1+1);
  ASSERT(buffer(5,0) == 5+5);
  ASSERT(buffer(5,1) == 5+5);
  ASSERT(buffer(6,0) == 6+6);
  ASSERT(buffer(6,1) == 6+6);
  ASSERT(buffer(7,0) == 7+7);
  ASSERT(buffer(7,1) == 7+7);
  ASSERT(buffer(8,0) == 8);
  ASSERT(buffer(8,1) == 8);
  ASSERT(buffer(11,0) == 11);
  ASSERT(buffer(11,1) == 11);
  ASSERT(buffer(12,0) == 12);
  ASSERT(buffer(12,1) == 12);
  ASSERT(buffer(13,0) == 13);
  ASSERT(buffer(13,1) == 13);
  ASSERT(buffer(14,0) == 14);
  ASSERT(buffer(14,1) == 14);
  ASSERT(buffer(19,0) == 19);
  ASSERT(buffer(19,1) == 19);
  ASSERT(buffer(20,0) == 20);
  ASSERT(buffer(20,1) == 20);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(add_stream.getStart() == mu::kIndefinite);
  ASSERT(add_stream.getEnd() == mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream3\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream1);
  add_stream.addSource(stream3);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(5,0) == 5);
  ASSERT(buffer(5,1) == 5);
  ASSERT(buffer(6,0) == 6+6);
  ASSERT(buffer(6,1) == 6+6);
  ASSERT(buffer(7,0) == 7+7);
  ASSERT(buffer(7,1) == 7+7);
  ASSERT(buffer(8,0) == 8+8);
  ASSERT(buffer(8,1) == 8+8);
  ASSERT(buffer(11,0) == 11+11);
  ASSERT(buffer(11,1) == 11+11);
  ASSERT(buffer(12,0) == 12+12);
  ASSERT(buffer(12,1) == 12+12);
  ASSERT(buffer(13,0) == 13+13);
  ASSERT(buffer(13,1) == 13+13);
  ASSERT(buffer(14,0) == 14);
  ASSERT(buffer(14,1) == 14);
  ASSERT(buffer(19,0) == 19);
  ASSERT(buffer(19,1) == 19);
  ASSERT(buffer(20,0) == 20);
  ASSERT(buffer(20,1) == 20);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(add_stream.getStart() == mu::kIndefinite);
  ASSERT(add_stream.getEnd() == mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream4\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream1);
  add_stream.addSource(stream4);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(5,0) == 5);
  ASSERT(buffer(5,1) == 5);
  ASSERT(buffer(6,0) == 6);
  ASSERT(buffer(6,1) == 6);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 8);
  ASSERT(buffer(8,1) == 8);
  ASSERT(buffer(11,0) == 11);
  ASSERT(buffer(11,1) == 11);
  ASSERT(buffer(12,0) == 12+12);
  ASSERT(buffer(12,1) == 12+12);
  ASSERT(buffer(13,0) == 13+13);
  ASSERT(buffer(13,1) == 13+13);
  ASSERT(buffer(14,0) == 14+14);
  ASSERT(buffer(14,1) == 14+14);
  ASSERT(buffer(19,0) == 19+19);
  ASSERT(buffer(19,1) == 19+19);
  ASSERT(buffer(20,0) == 20);
  ASSERT(buffer(20,1) == 20);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  ASSERT(add_stream.getStart() == mu::kIndefinite);
  ASSERT(add_stream.getEnd() == mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== stream2 + stream3\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream2);
  add_stream.addSource(stream3);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(5,0) == 5);
  ASSERT(buffer(5,1) == 5);
  ASSERT(buffer(6,0) == 6+6);
  ASSERT(buffer(6,1) == 6+6);
  ASSERT(buffer(7,0) == 7+7);
  ASSERT(buffer(7,1) == 7+7);
  ASSERT(buffer(8,0) == 8);
  ASSERT(buffer(8,1) == 8);
  ASSERT(buffer(11,0) == 11);
  ASSERT(buffer(11,1) == 11);
  ASSERT(buffer(12,0) == 12);
  ASSERT(buffer(12,1) == 12);
  ASSERT(buffer(13,0) == 13);
  ASSERT(buffer(13,1) == 13);
  ASSERT(buffer(14,0) == 0);
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer(19,0) == 0);
  ASSERT(buffer(19,1) == 0);
  ASSERT(buffer(20,0) == 0);
  ASSERT(buffer(20,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 0);
  ASSERT(add_stream.getEnd() == 14);

  // ================================================================
  fprintf(stderr, "=== stream2 + stream4\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream2);
  add_stream.addSource(stream4);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(5,0) == 5);
  ASSERT(buffer(5,1) == 5);
  ASSERT(buffer(6,0) == 6);
  ASSERT(buffer(6,1) == 6);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 0);
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer(11,0) == 0);
  ASSERT(buffer(11,1) == 0);
  ASSERT(buffer(12,0) == 12);
  ASSERT(buffer(12,1) == 12);
  ASSERT(buffer(13,0) == 13);
  ASSERT(buffer(13,1) == 13);
  ASSERT(buffer(14,0) == 14);
  ASSERT(buffer(14,1) == 14);
  ASSERT(buffer(19,0) == 19);
  ASSERT(buffer(19,1) == 19);
  ASSERT(buffer(20,0) == 0);
  ASSERT(buffer(20,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 0);
  ASSERT(add_stream.getEnd() == 20);

  // ================================================================
  fprintf(stderr, "=== stream3 + stream4\n");
  add_stream.removeAllSources();
  add_stream.addSource(stream3);
  add_stream.addSource(stream4);
  add_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(5,0) == 0);
  ASSERT(buffer(5,1) == 0);
  ASSERT(buffer(6,0) == 6);
  ASSERT(buffer(6,1) == 6);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 8);
  ASSERT(buffer(8,1) == 8);
  ASSERT(buffer(11,0) == 11);
  ASSERT(buffer(11,1) == 11);
  ASSERT(buffer(12,0) == 12+12);
  ASSERT(buffer(12,1) == 12+12);
  ASSERT(buffer(13,0) == 13+13);
  ASSERT(buffer(13,1) == 13+13);
  ASSERT(buffer(14,0) == 14);
  ASSERT(buffer(14,1) == 14);
  ASSERT(buffer(19,0) == 19);
  ASSERT(buffer(19,1) == 19);
  ASSERT(buffer(20,0) == 0);
  ASSERT(buffer(20,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(add_stream.getStart() == 6);
  ASSERT(add_stream.getEnd() == 20);

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
