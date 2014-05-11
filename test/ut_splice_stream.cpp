/*
 * Unit Test SpliceStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "identity_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "splice_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::IdentityStream identity_stream;
  mu::SpliceStream splice_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  /*
   * configuration:  Each stream starts at value = 0 and ramps linearly:
   * stream 1: start indefinite, end indefinite
   * stream 2: start frame 2, end frame 10
   * stream 3: start frame 8, end frame 16
   * stream 4: start frame 14, end frame 22
   *
   * nb: stream3 overlaps stream2 and stream4.  
   *     stream2 and stream4 are disjoint.
   */
  mu::Stream *stream1 = new mu::IdentityStream();
  mu::Stream *stream2 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(0).setEnd(8);
  mu::Stream *stream3 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(0).setEnd(8);
  mu::Stream *stream4 = &(new mu::CropStream())->setSource(new mu::IdentityStream()).setStart(0).setEnd(8);

  fprintf(stderr, "=== assay...\n");
  fprintf(stderr, "stream1->getStart()=%ld, getEnd()=%ld\n", stream1->getStart(), stream1->getEnd());
  fprintf(stderr, "stream2->getStart()=%ld, getEnd()=%ld\n", stream2->getStart(), stream2->getEnd());
  fprintf(stderr, "stream3->getStart()=%ld, getEnd()=%ld\n", stream3->getStart(), stream3->getEnd());
  fprintf(stderr, "stream4->getStart()=%ld, getEnd()=%ld\n", stream4->getStart(), stream4->getEnd());

  // ================================================================
  fprintf(stderr, "=== verify stream2\n");
  ASSERT(stream2->getStart() == 0);
  ASSERT(stream2->getEnd() == 8);

  stream2->step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);     // stream 2 starts
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 0);    // stream 2 ends
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== no inputs\n");
  splice_stream.removeAllSources();

  ASSERT(splice_stream.getStart()==mu::kIndefinite);
  ASSERT(splice_stream.getEnd()==mu::kIndefinite);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream1 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1, 0);

  ASSERT(splice_stream.getStart()==mu::kIndefinite);
  ASSERT(splice_stream.getEnd()==mu::kIndefinite);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == (FRAME_COUNT-1));
  ASSERT(buffer((FRAME_COUNT-1),1) == (FRAME_COUNT-1));

  // ================================================================
  fprintf(stderr, "=== stream2 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream2, 2);

  ASSERT(splice_stream.getStart() == 2);
  ASSERT(splice_stream.getEnd() == 10);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(1,1) == 0);
  ASSERT(buffer(2,0) == 0);     // stream 2 starts
  ASSERT(buffer(2,1) == 0);
  ASSERT(buffer(3,0) == 1);
  ASSERT(buffer(3,1) == 1);
  ASSERT(buffer(9,0) == 7);
  ASSERT(buffer(9,1) == 7);
  ASSERT(buffer(10,0) == 0);    // stream 2 ends
  ASSERT(buffer(10,1) == 0);
  ASSERT(buffer(11,0) == 0);
  ASSERT(buffer(11,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream3 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream3, 8);

  ASSERT(splice_stream.getStart() == 8);
  ASSERT(splice_stream.getEnd() == 16);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(7,0) == 0);
  ASSERT(buffer(7,1) == 0);
  ASSERT(buffer(8,0) == 0);     // start of stream 3
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer(9,0) == 1);
  ASSERT(buffer(9,1) == 1);
  ASSERT(buffer(15,0) == 7);
  ASSERT(buffer(15,1) == 7);
  ASSERT(buffer(16,0) == 0);    // end of stream 3
  ASSERT(buffer(16,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream4 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream4, 14);

  ASSERT(splice_stream.getStart() == 14);
  ASSERT(splice_stream.getEnd() == 22);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(13,0) == 0);
  ASSERT(buffer(13,1) == 0);
  ASSERT(buffer(14,0) == 0);    // stream 4 starts
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer(15,0) == 1);
  ASSERT(buffer(15,1) == 1);
  ASSERT(buffer(21,0) == 7);
  ASSERT(buffer(21,1) == 7);
  ASSERT(buffer(22,0) == 0);    // stream 4 ends
  ASSERT(buffer(22,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream2\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1, 0);
  splice_stream.addSource(stream2, 2);

  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == mu::kIndefinite);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);     // stream 1 in progress...
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer(2,0) == 0);     // stream 2 starts
  ASSERT(buffer(2,1) == 0);
  ASSERT(buffer(3,0) == 1);
  ASSERT(buffer(3,1) == 1);
  ASSERT(buffer(9,0) == 7);
  ASSERT(buffer(9,1) == 7);
  ASSERT(buffer(10,0) == 0);    // stream 2 ends
  ASSERT(buffer(10,1) == 0);
  ASSERT(buffer(11,0) == 0);
  ASSERT(buffer(11,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream3\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1, 0);
  splice_stream.addSource(stream3, 8);

  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == mu::kIndefinite);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);     // stream 1 in progress
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer(5,0) == 5);
  ASSERT(buffer(5,1) == 5);
  ASSERT(buffer(7,0) == 7);
  ASSERT(buffer(7,1) == 7);
  ASSERT(buffer(8,0) == 0);     // stream 3 starts
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer(9,0) == 1);
  ASSERT(buffer(9,1) == 1);
  ASSERT(buffer(15,0) == 7);
  ASSERT(buffer(15,1) == 7);
  ASSERT(buffer(16,0) == 0);   // stream 3 ends
  ASSERT(buffer(16,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1, 0);
  splice_stream.addSource(stream4, 14);

  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == mu::kIndefinite);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);     // stream 1 in progress
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  ASSERT(buffer(13,0) == 13);
  ASSERT(buffer(13,1) == 13);
  ASSERT(buffer(14,0) == 0);   // stream 4 starts
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer(15,0) == 1);
  ASSERT(buffer(15,1) == 1);
  ASSERT(buffer(21,0) == 7);
  ASSERT(buffer(21,1) == 7);
  ASSERT(buffer(22,0) == 0);   // stream 4 ends
  ASSERT(buffer(22,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream2 + stream3\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream2, 2);
  splice_stream.addSource(stream3, 8);

  ASSERT(splice_stream.getStart() == 2);
  ASSERT(splice_stream.getEnd() == 16);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(1,1) == 0);
  ASSERT(buffer(2,0) == 0);     // stream 2 starts
  ASSERT(buffer(2,1) == 0);
  ASSERT(buffer(3,0) == 1);
  ASSERT(buffer(3,1) == 1);
  ASSERT(buffer(7,0) == 5);
  ASSERT(buffer(7,1) == 5);
  ASSERT(buffer(8,0) == 0);   // stream 3 starts
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer(9,0) == 1);
  ASSERT(buffer(9,1) == 1);
  ASSERT(buffer(15,0) == 7);
  ASSERT(buffer(15,1) == 7);
  ASSERT(buffer(16,0) == 0);    // stream 3 ends
  ASSERT(buffer(16,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream2 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream2, 2);
  splice_stream.addSource(stream4, 14);

  ASSERT(splice_stream.getStart() == 2);
  ASSERT(splice_stream.getEnd() == 22);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(1,1) == 0);

  ASSERT(buffer(2,0) == 0);     // stream 2 starts
  ASSERT(buffer(2,1) == 0);
  ASSERT(buffer(3,0) == 1);
  ASSERT(buffer(3,1) == 1);
  ASSERT(buffer(9,0) == 7);
  ASSERT(buffer(9,1) == 7);
  ASSERT(buffer(10,0) == 0);    // stream 2 ends
  ASSERT(buffer(10,1) == 0);

  ASSERT(buffer(13,0) == 0);
  ASSERT(buffer(13,1) == 0);
  ASSERT(buffer(14,0) == 0);    // stream 4 starts
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer(15,0) == 1);
  ASSERT(buffer(15,1) == 1);
  ASSERT(buffer(21,0) == 7);
  ASSERT(buffer(21,1) == 7);
  ASSERT(buffer(22,0) == 0);    // stream 4 ends
  ASSERT(buffer(22,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream3 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream3, 8);
  splice_stream.addSource(stream4, 14);

  ASSERT(splice_stream.getStart() == 8);
  ASSERT(splice_stream.getEnd() == 22);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 0);
  ASSERT(buffer(1,1) == 0);

  ASSERT(buffer(8,0) == 0);     // stream 3 starts
  ASSERT(buffer(8,1) == 0);
  ASSERT(buffer(9,0) == 1);
  ASSERT(buffer(9,1) == 1);
  ASSERT(buffer(13,0) == 5);
  ASSERT(buffer(13,1) == 5);
  ASSERT(buffer(14,0) == 0);    // stream 4 starts
  ASSERT(buffer(14,1) == 0);
  ASSERT(buffer(15,0) == 1);
  ASSERT(buffer(15,1) == 1);
  ASSERT(buffer(21,0) == 7);
  ASSERT(buffer(21,1) == 7);
  ASSERT(buffer(22,0) == 0);    // stream 4 ends
  ASSERT(buffer(22,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
