/*
 * Unit Test ResampleStream
 */
#include "assert.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_stream.h"
#include "linseg_stream.h"
#include "resample_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::IdentityStream identity_stream;
  mu::LinsegStream linseg_stream;
  mu::ResampleStream resample_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);


  fprintf(stderr,"=== null inputs\n");
  resample_stream.setSampleSource(NULL);
  resample_stream.setTimingSource(NULL);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== null sample input\n");
  resample_stream.setSampleSource(NULL);
  resample_stream.setTimingSource(&linseg_stream);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== null timing input\n");
  resample_stream.setSampleSource(&identity_stream);
  resample_stream.setTimingSource(NULL);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== unity timing\n");
  
  resample_stream.setSampleSource(&identity_stream);
  resample_stream.setTimingSource(&identity_stream);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 1.0);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 1.0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== 2x timing\n");
  
  linseg_stream.removeAllBreakpoints();
  linseg_stream.addBreakpoint(0, 0.0);
  linseg_stream.addBreakpoint(512, 1024.0);

  resample_stream.setSampleSource(&identity_stream);
  resample_stream.setTimingSource(&linseg_stream);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 2.0);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 2.0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);


  fprintf(stderr,"=== 0.5x timing\n");
  
  linseg_stream.removeAllBreakpoints();
  linseg_stream.addBreakpoint(0, 0.0);
  linseg_stream.addBreakpoint(512, 256.0);

  resample_stream.setSampleSource(&identity_stream);
  resample_stream.setTimingSource(&linseg_stream);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 0.5);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 0.5);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);


  fprintf(stderr,"=== -1.0x timing\n");
  
  linseg_stream.removeAllBreakpoints();
  linseg_stream.addBreakpoint(0, 511.0);
  linseg_stream.addBreakpoint(511, 0.0);

//   linseg_stream.step(buffer, 0, player);
//   for (int i=0; i<buffer.frames(); i++) {
//     fprintf(stderr,"linseg[%i] = %f\n", i, buffer(i,0));
//   }

  resample_stream.setSampleSource(&identity_stream);
  resample_stream.setTimingSource(&linseg_stream);
  resample_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == (buffer.frames() - 1));
  ASSERT(buffer(0,1) == (buffer.frames() - 1));

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == 0.0);
  ASSERT(buffer(buffer.frames()-1,1) == 0.0);

  ASSERT(resample_stream.getStart() == mu::kIndefinite);
  ASSERT(resample_stream.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== done\n");
  return 0;

}
