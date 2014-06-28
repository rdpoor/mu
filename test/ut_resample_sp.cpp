/*
 * Unit Test ResampleSP
 */
#include "assert.h"
#include "mu.h"
#include "nrt_player.h"
#include "identity_sp.h"
#include "linseg_sp.h"
#include "resample_sp.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::IdentitySP identity_sp;
  mu::LinsegSP linseg_sp;
  mu::ResampleSP resample_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);


  fprintf(stderr,"=== null inputs\n");
  resample_sp.setSampleSource(NULL);
  resample_sp.setTimingSource(NULL);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== null sample input\n");
  resample_sp.setSampleSource(NULL);
  resample_sp.setTimingSource(&linseg_sp);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== null timing input\n");
  resample_sp.setSampleSource(&identity_sp);
  resample_sp.setTimingSource(NULL);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== unity timing\n");
  
  resample_sp.setSampleSource(&identity_sp);
  resample_sp.setTimingSource(&identity_sp);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 1.0);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 1.0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== 2x timing\n");
  
  linseg_sp.removeAllBreakpoints();
  linseg_sp.addBreakpoint(0, 0.0);
  linseg_sp.addBreakpoint(512, 1024.0);

  resample_sp.setSampleSource(&identity_sp);
  resample_sp.setTimingSource(&linseg_sp);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 2.0);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 2.0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);


  fprintf(stderr,"=== 0.5x timing\n");
  
  linseg_sp.removeAllBreakpoints();
  linseg_sp.addBreakpoint(0, 0.0);
  linseg_sp.addBreakpoint(512, 256.0);

  resample_sp.setSampleSource(&identity_sp);
  resample_sp.setTimingSource(&linseg_sp);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer(0,1) == 0.0);

  ASSERT(buffer(buffer.frames()-1,0) == (buffer.frames()-1) * 0.5);
  ASSERT(buffer(buffer.frames()-1,1) == (buffer.frames()-1) * 0.5);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);


  fprintf(stderr,"=== -1.0x timing\n");
  
  linseg_sp.removeAllBreakpoints();
  linseg_sp.addBreakpoint(0, 511.0);
  linseg_sp.addBreakpoint(511, 0.0);

//   linseg_sp.step(buffer, 0, true);
//   for (int i=0; i<buffer.frames(); i++) {
//     fprintf(stderr,"linseg[%i] = %f\n", i, buffer(i,0));
//   }

  resample_sp.setSampleSource(&identity_sp);
  resample_sp.setTimingSource(&linseg_sp);
  resample_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == (buffer.frames() - 1));
  ASSERT(buffer(0,1) == (buffer.frames() - 1));

  fprintf(stderr, "buffer(511,0) = %f\n", buffer(511,0));
  ASSERT(buffer(buffer.frames()-1,0) == 0.0);
  ASSERT(buffer(buffer.frames()-1,1) == 0.0);

  ASSERT(resample_sp.getStart() == mu::kIndefinite);
  ASSERT(resample_sp.getEnd() == mu::kIndefinite);

  fprintf(stderr,"=== done\n");
  return 0;

}
