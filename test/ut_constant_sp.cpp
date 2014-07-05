/*
 * Unit Test ConstantSP
 */
#include "assert.h"
#include "constant_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::ConstantSP constant_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  constant_sp.setValue(42.0); // scott addams
  constant_sp.step(buffer, 0, true);
  
  ASSERT(buffer(0,0) == 42.0);
  ASSERT(buffer(0,1) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 42.0);

  constant_sp.step(buffer, 12345678, true);

  ASSERT(buffer(0,0) == 42.0);
  ASSERT(buffer(0,1) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 42.0);

  ASSERT(constant_sp.getStart() == mu::TickUtils::indefinite());
  ASSERT(constant_sp.getEnd() == mu::TickUtils::indefinite());

  return 0;

}
