/*
 * Unit Test LinsegSP
 */
#include "assert.h"
#include "linseg_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 1

int main() {
  mu::LinsegSP linseg_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  fprintf(stderr, "=== no breakpoints\n");
  ASSERT(linseg_sp.getBreakpointCount() == 0);

  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  linseg_sp.step(buffer, 0, true);
  ASSERT(buffer(0,0) == 0);

  fprintf(stderr, "=== one breakpoint, no hold\n");
  linseg_sp.setHoldValue(false);
  linseg_sp.addBreakpoint(10, 10);
  ASSERT(linseg_sp.getBreakpointCount() == 1);

  ASSERT(linseg_sp.getStart() == 10);
  ASSERT(linseg_sp.getEnd() == 10);

  linseg_sp.step(buffer, 0, true);
  ASSERT(buffer(9,0) == 0);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(11,0) == 0);

  fprintf(stderr, "=== one breakpoint, hold value\n");
  linseg_sp.setHoldValue(true);
  
  ASSERT(linseg_sp.getBreakpointCount() == 1);

  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  linseg_sp.step(buffer, 0, true);
  ASSERT(buffer(9,0) == 10);
  ASSERT(buffer(10,0) == 10);
  ASSERT(buffer(11,0) == 10);

  fprintf(stderr, "=== two breakpoints, no hold\n");
  linseg_sp.setHoldValue(false);
  linseg_sp.addBreakpoint(20, 11);
  ASSERT(linseg_sp.getBreakpointCount() == 2);

  ASSERT(linseg_sp.getStart() == 10);
  ASSERT(linseg_sp.getEnd() == 20);

  linseg_sp.step(buffer, 0, true);
  ASSERT(buffer(9,0) == 0.0);
  ASSERT(buffer(10,0) == 10.0);
  ASSERT(buffer(11,0) == 10.1);
  ASSERT(buffer(12,0) == 10.2);
  ASSERT(buffer(13,0) == 10.3);
  ASSERT(buffer(14,0) == 10.4);
  ASSERT(buffer(15,0) == 10.5);
  ASSERT(buffer(16,0) == 10.6);
  ASSERT(buffer(17,0) == 10.7);
  ASSERT(buffer(18,0) == 10.8);
  ASSERT(buffer(19,0) == 10.9);
  ASSERT(buffer(20,0) == 11.0);
  ASSERT(buffer(21,0) == 0.0);

  fprintf(stderr, "=== two breakpoints, hold value\n");
  linseg_sp.setHoldValue(true);
  ASSERT(linseg_sp.getBreakpointCount() == 2);

  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  linseg_sp.step(buffer, 0, true);
  ASSERT(buffer(9,0) == 10.0);
  ASSERT(buffer(10,0) == 10.0);
  ASSERT(buffer(11,0) == 10.1);
  ASSERT(buffer(12,0) == 10.2);
  ASSERT(buffer(13,0) == 10.3);
  ASSERT(buffer(14,0) == 10.4);
  ASSERT(buffer(15,0) == 10.5);
  ASSERT(buffer(16,0) == 10.6);
  ASSERT(buffer(17,0) == 10.7);
  ASSERT(buffer(18,0) == 10.8);
  ASSERT(buffer(19,0) == 10.9);
  ASSERT(buffer(20,0) == 11.0);
  ASSERT(buffer(21,0) == 11.0);

  fprintf(stderr, "=== three breakpoints, no hold\n");
  linseg_sp.setHoldValue(false);
  linseg_sp.addBreakpoint(-10, 8.0);
  ASSERT(linseg_sp.getBreakpointCount() == 3);

  ASSERT(linseg_sp.getStart() == -10);
  ASSERT(linseg_sp.getEnd() == 20);

  linseg_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 9.0);
  ASSERT(buffer(1,0) == 9.1);
  ASSERT(buffer(2,0) == 9.2);
  ASSERT(buffer(3,0) == 9.3);
  ASSERT(buffer(4,0) == 9.4);
  ASSERT(buffer(5,0) == 9.5);
  ASSERT(buffer(6,0) == 9.6);
  ASSERT(buffer(7,0) == 9.7);
  ASSERT(buffer(8,0) == 9.8);
  ASSERT(buffer(9,0) == 9.9);
  ASSERT(buffer(10,0) == 10.0);
  ASSERT(buffer(11,0) == 10.1);
  ASSERT(buffer(12,0) == 10.2);
  ASSERT(buffer(13,0) == 10.3);
  ASSERT(buffer(14,0) == 10.4);
  ASSERT(buffer(15,0) == 10.5);
  ASSERT(buffer(16,0) == 10.6);
  ASSERT(buffer(17,0) == 10.7);
  ASSERT(buffer(18,0) == 10.8);
  ASSERT(buffer(19,0) == 10.9);
  ASSERT(buffer(20,0) == 11.0);
  ASSERT(buffer(21,0) == 0.0);

  fprintf(stderr, "=== three breakpoints, spanning buffer, no hold\n");
  
  ASSERT(linseg_sp.getStart() == -10);
  ASSERT(linseg_sp.getEnd() == 20);

  linseg_sp.step(buffer, -FRAME_COUNT, true);
  ASSERT(buffer(FRAME_COUNT-1,0) == 8.9);
  ASSERT(buffer(FRAME_COUNT-2,0) == 8.8);
  ASSERT(buffer(FRAME_COUNT-3,0) == 8.7);
  ASSERT(buffer(FRAME_COUNT-4,0) == 8.6);
  ASSERT(buffer(FRAME_COUNT-5,0) == 8.5);
  ASSERT(buffer(FRAME_COUNT-6,0) == 8.4);
  ASSERT(buffer(FRAME_COUNT-7,0) == 8.3);
  ASSERT(buffer(FRAME_COUNT-8,0) == 8.2);
  ASSERT(buffer(FRAME_COUNT-9,0) == 8.1);
  ASSERT(buffer(FRAME_COUNT-10,0) == 8.0);
  ASSERT(buffer(FRAME_COUNT-11,0) == 0.0);

  fprintf(stderr, "=== three breakpoints, hold value\n");
  linseg_sp.setHoldValue(true);
  ASSERT(linseg_sp.getBreakpointCount() == 3);

  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  linseg_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 9.0);
  ASSERT(buffer(1,0) == 9.1);
  ASSERT(buffer(2,0) == 9.2);
  ASSERT(buffer(3,0) == 9.3);
  ASSERT(buffer(4,0) == 9.4);
  ASSERT(buffer(5,0) == 9.5);
  ASSERT(buffer(6,0) == 9.6);
  ASSERT(buffer(7,0) == 9.7);
  ASSERT(buffer(8,0) == 9.8);
  ASSERT(buffer(9,0) == 9.9);
  ASSERT(buffer(10,0) == 10.0);
  ASSERT(buffer(11,0) == 10.1);
  ASSERT(buffer(12,0) == 10.2);
  ASSERT(buffer(13,0) == 10.3);
  ASSERT(buffer(14,0) == 10.4);
  ASSERT(buffer(15,0) == 10.5);
  ASSERT(buffer(16,0) == 10.6);
  ASSERT(buffer(17,0) == 10.7);
  ASSERT(buffer(18,0) == 10.8);
  ASSERT(buffer(19,0) == 10.9);
  ASSERT(buffer(20,0) == 11.0);
  ASSERT(buffer(21,0) == 11.0);

  fprintf(stderr, "=== three breakpoints, spanning buffer, hold value\n");
  
  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  linseg_sp.step(buffer, -FRAME_COUNT, true);
  ASSERT(buffer(FRAME_COUNT-1,0) == 8.9);
  ASSERT(buffer(FRAME_COUNT-2,0) == 8.8);
  ASSERT(buffer(FRAME_COUNT-3,0) == 8.7);
  ASSERT(buffer(FRAME_COUNT-4,0) == 8.6);
  ASSERT(buffer(FRAME_COUNT-5,0) == 8.5);
  ASSERT(buffer(FRAME_COUNT-6,0) == 8.4);
  ASSERT(buffer(FRAME_COUNT-7,0) == 8.3);
  ASSERT(buffer(FRAME_COUNT-8,0) == 8.2);
  ASSERT(buffer(FRAME_COUNT-9,0) == 8.1);
  ASSERT(buffer(FRAME_COUNT-10,0) == 8.0);
  ASSERT(buffer(FRAME_COUNT-11,0) == 8.0);

  fprintf(stderr, "=== remove all breakpoints\n");
  linseg_sp.removeAllBreakpoints();
  ASSERT(linseg_sp.getBreakpointCount() == 0);

  ASSERT(linseg_sp.getStart() == mu::kIndefinite);
  ASSERT(linseg_sp.getEnd() == mu::kIndefinite);

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
