/*
 * Unit Test SpliceStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "identity_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "splice_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

#define S2S 2
#define S2E 10
#define S3S 8
#define S3E 16
#define S4S 14
#define S4E 22

int main() {
  mu::IdentityStream identity_stream;
  mu::SpliceStream splice_stream;
  mu::CropStream crop_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  /*
   * configuration:  Each stream starts at value = 0 and ramps linearly:
   * stream 1: start indefinite, end indefinite (value = 0 at tick = 0)
   * stream 2: start frame 2, end frame 10
   * stream 3: start frame 8, end frame 16
   * stream 4: start frame 14, end frame 22
   *
   * nb: stream3 overlaps stream2 and stream4.  
   *     stream2 and stream4 are disjoint.
   */
  
  crop_stream.setSource(&identity_stream).setStart(0).setEnd(8);

  mu::Stream *stream1 = &identity_stream;
  mu::Stream *stream2 = &(new mu::DelayStream())->setDelay(S2S).setSource(&crop_stream);
  mu::Stream *stream3 = &(new mu::DelayStream())->setDelay(S3S).setSource(&crop_stream);
  mu::Stream *stream4 = &(new mu::DelayStream())->setDelay(S4S).setSource(&crop_stream);

  fprintf(stderr, "=== assay...\n");
  fprintf(stderr, "stream1->getStart()=%ld, getEnd()=%ld\n", stream1->getStart(), stream1->getEnd());
  fprintf(stderr, "stream2->getStart()=%ld, getEnd()=%ld\n", stream2->getStart(), stream2->getEnd());
  fprintf(stderr, "stream3->getStart()=%ld, getEnd()=%ld\n", stream3->getStart(), stream3->getEnd());
  fprintf(stderr, "stream4->getStart()=%ld, getEnd()=%ld\n", stream4->getStart(), stream4->getEnd());

  // ================================================================
  fprintf(stderr, "=== verify stream2\n");
  ASSERT(stream2->getStart() == S2S);
  ASSERT(stream2->getEnd() == S2E);

  stream2->step(buffer, 0, player);

  ASSERT(buffer(S2S-1,0) == 0);
  ASSERT(buffer(S2S-1,1) == 0);
  ASSERT(buffer(S2S+0,0) == 0); // stream 2 starts
  ASSERT(buffer(S2S+0,1) == 0);
  ASSERT(buffer(S2S+1,0) == 1); // stream 2 starts
  ASSERT(buffer(S2S+1,1) == 1);
  ASSERT(buffer(S2E-1,0) == (S2E-S2S-1));
  ASSERT(buffer(S2E-1,1) == (S2E-S2S-1));
  ASSERT(buffer(S2E+0,0) == 0); // stream 2 ends
  ASSERT(buffer(S2E+0,1) == 0);

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
  splice_stream.addSource(stream1);

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
  splice_stream.addSource(stream2);

  ASSERT(splice_stream.getStart() == S2S);
  ASSERT(splice_stream.getEnd() == S2E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S2S-1,0) == 0);
  ASSERT(buffer(S2S-1,1) == 0);
  ASSERT(buffer(S2S+0,0) == 0); // stream 2 starts
  ASSERT(buffer(S2S+0,1) == 0);
  ASSERT(buffer(S2S+1,0) == 1); // stream 2 starts
  ASSERT(buffer(S2S+1,1) == 1);
  ASSERT(buffer(S2E-1,0) == (S2E-S2S-1));
  ASSERT(buffer(S2E-1,1) == (S2E-S2S-1));
  ASSERT(buffer(S2E+0,0) == 0); // stream 2 ends
  ASSERT(buffer(S2E+0,1) == 0);

  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream3 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream3);

  ASSERT(splice_stream.getStart() == S3S);
  ASSERT(splice_stream.getEnd() == S3E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S3S-1,0) == 0);
  ASSERT(buffer(S3S-1,1) == 0);
  ASSERT(buffer(S3S+0,0) == 0); // stream 3 starts
  ASSERT(buffer(S3S+0,1) == 0);
  ASSERT(buffer(S3S+1,0) == 1); // stream 3 starts
  ASSERT(buffer(S3S+1,1) == 1);
  ASSERT(buffer(S3E-1,0) == (S3E-S3S-1));
  ASSERT(buffer(S3E-1,1) == (S3E-S3S-1));
  ASSERT(buffer(S3E+0,0) == 0); // stream 3 ends
  ASSERT(buffer(S3E+0,1) == 0);

  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream4 only\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream4);

  ASSERT(splice_stream.getStart() == S4S);
  ASSERT(splice_stream.getEnd() == S4E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S4S-1,0) == 0);
  ASSERT(buffer(S4S-1,1) == 0);
  ASSERT(buffer(S4S+0,0) == 0); // stream 4 starts
  ASSERT(buffer(S4S+0,1) == 0);
  ASSERT(buffer(S4S+1,0) == 1); // stream 4 starts
  ASSERT(buffer(S4S+1,1) == 1);
  ASSERT(buffer(S4E-1,0) == (S4E-S4S-1));
  ASSERT(buffer(S4E-1,1) == (S4E-S4S-1));
  ASSERT(buffer(S4E+0,0) == 0); // stream 4 ends
  ASSERT(buffer(S4E+0,1) == 0);

  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream1 + stream2\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1);
  splice_stream.addSource(stream2);

  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == S2E);

  splice_stream.step(buffer, 0, player);
  
  ASSERT(buffer(0,0) == 0);     // stream 1 in progress...
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  
  ASSERT(buffer(S2S+0,0) == 0); // stream 2 starts
  ASSERT(buffer(S2S+0,1) == 0);
  ASSERT(buffer(S2S+1,0) == 1); // stream 2 starts
  ASSERT(buffer(S2S+1,1) == 1);
  ASSERT(buffer(S2E-1,0) == (S2E-S2S-1));
  ASSERT(buffer(S2E-1,1) == (S2E-S2S-1));
  ASSERT(buffer(S2E+0,0) == 0); // stream 2 ends
  ASSERT(buffer(S2E+0,1) == 0);
  
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);
  
  // ================================================================
  fprintf(stderr, "=== stream1 + stream3\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1);
  splice_stream.addSource(stream3);
  
  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == S3E);
  
  splice_stream.step(buffer, 0, player);
  
  ASSERT(buffer(0,0) == 0);     // stream 1 in progress
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  
  ASSERT(buffer(S3S+0,0) == 0); // stream 3 starts
  ASSERT(buffer(S3S+0,1) == 0);
  ASSERT(buffer(S3S+1,0) == 1); // stream 3 starts
  ASSERT(buffer(S3S+1,1) == 1);
  ASSERT(buffer(S3E-1,0) == (S3E-S3S-1));
  ASSERT(buffer(S3E-1,1) == (S3E-S3S-1));
  ASSERT(buffer(S3E+0,0) == 0); // stream 3 ends
  ASSERT(buffer(S3E+0,1) == 0);
  
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);
  
  // ================================================================
  fprintf(stderr, "=== stream1 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream1);
  splice_stream.addSource(stream4);

  ASSERT(splice_stream.getStart() == mu::kIndefinite);
  ASSERT(splice_stream.getEnd() == S4E);

  splice_stream.step(buffer, 0, player);
  
  ASSERT(buffer(0,0) == 0);     // stream 1 in progress
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(1,0) == 1);
  ASSERT(buffer(1,1) == 1);
  
  ASSERT(buffer(S4S+0,0) == 0); // stream 4 starts
  ASSERT(buffer(S4S+0,1) == 0);
  ASSERT(buffer(S4S+1,0) == 1); // stream 4 starts
  ASSERT(buffer(S4S+1,1) == 1);
  ASSERT(buffer(S4E-1,0) == (S4E-S4S-1));
  ASSERT(buffer(S4E-1,1) == (S4E-S4S-1));
  ASSERT(buffer(S4E+0,0) == 0); // stream 4 ends
  ASSERT(buffer(S4E+0,1) == 0);
  
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);
  
  // ================================================================
  fprintf(stderr, "=== stream2 + stream3\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream2);
  splice_stream.addSource(stream3);
  
  ASSERT(splice_stream.getStart() == S2S);
  ASSERT(splice_stream.getEnd() == S3E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S2S-1,0) == 0);
  ASSERT(buffer(S2S-1,1) == 0);
  ASSERT(buffer(S2S+0,0) == 0); // stream 2 starts
  ASSERT(buffer(S2S+0,1) == 0);
  ASSERT(buffer(S2S+1,0) == 1); // stream 2 starts
  ASSERT(buffer(S2S+1,1) == 1);

  // assumes knowledge of overlap
  ASSERT(buffer(S3S-1,0) == S3S-S2S-1);
  ASSERT(buffer(S3S-1,1) == S3S-S2S-1);

  ASSERT(buffer(S3S+0,0) == 0); // stream 3 starts
  ASSERT(buffer(S3S+0,1) == 0);
  ASSERT(buffer(S3S+1,0) == 1); // stream 3 starts
  ASSERT(buffer(S3S+1,1) == 1);
  ASSERT(buffer(S3E-1,0) == (S3E-S3S-1));
  ASSERT(buffer(S3E-1,1) == (S3E-S3S-1));
  ASSERT(buffer(S3E+0,0) == 0); // stream 3 ends
  ASSERT(buffer(S3E+0,1) == 0);

  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream2 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream2);
  splice_stream.addSource(stream4);

  ASSERT(splice_stream.getStart() == S2S);
  ASSERT(splice_stream.getEnd() == S4E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S2S+0,0) == 0); // stream 2 starts
  ASSERT(buffer(S2S+0,1) == 0);
  ASSERT(buffer(S2S+1,0) == 1); // stream 2 starts
  ASSERT(buffer(S2S+1,1) == 1);
  ASSERT(buffer(S2E-1,0) == (S2E-S2S-1));
  ASSERT(buffer(S2E-1,1) == (S2E-S2S-1));
  ASSERT(buffer(S2E+0,0) == 0); // stream 2 ends
  ASSERT(buffer(S2E+0,1) == 0);
  
  ASSERT(buffer(S4S+0,0) == 0); // stream 4 starts
  ASSERT(buffer(S4S+0,1) == 0);
  ASSERT(buffer(S4S+1,0) == 1); // stream 4 starts
  ASSERT(buffer(S4S+1,1) == 1);
  ASSERT(buffer(S4E-1,0) == (S4E-S4S-1));
  ASSERT(buffer(S4E-1,1) == (S4E-S4S-1));
  ASSERT(buffer(S4E+0,0) == 0); // stream 4 ends
  ASSERT(buffer(S4E+0,1) == 0);
  
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr, "=== stream3 + stream4\n");
  splice_stream.removeAllSources();
  splice_stream.addSource(stream3);
  splice_stream.addSource(stream4);

  ASSERT(splice_stream.getStart() ==S3S);
  ASSERT(splice_stream.getEnd() == S4E);

  splice_stream.step(buffer, 0, player);

  ASSERT(buffer(S3S+0,0) == 0); // stream 3 starts
  ASSERT(buffer(S3S+0,1) == 0);
  ASSERT(buffer(S3S+1,0) == 1); // stream 3 starts
  ASSERT(buffer(S3S+1,1) == 1);
  
  ASSERT(buffer(S4S-1,0) == S4S-S3S-1);
  ASSERT(buffer(S4S-1,1) == S4S-S3S-1);

  ASSERT(buffer(S4S+0,0) == 0); // stream 4 starts
  ASSERT(buffer(S4S+0,1) == 0);
  ASSERT(buffer(S4S+1,0) == 1); // stream 4 starts
  ASSERT(buffer(S4S+1,1) == 1);
  ASSERT(buffer(S4E-1,0) == (S4E-S4S-1));
  ASSERT(buffer(S4E-1,1) == (S4E-S4S-1));
  ASSERT(buffer(S4E+0,0) == 0); // stream 4 ends
  ASSERT(buffer(S4E+0,1) == 0);
  
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
