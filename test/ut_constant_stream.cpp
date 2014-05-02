/*
 * Unit Test ConstantStream
 */
#include "assert.h"
#include "constant_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::ConstantStream constant_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  constant_stream.setValue(42.0); // scott addams
  constant_stream.step(buffer, 0, player);
  
  ASSERT(buffer(0,0) == 42.0);
  ASSERT(buffer(0,1) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 42.0);

  constant_stream.step(buffer, 12345678, player);

  ASSERT(buffer(0,0) == 42.0);
  ASSERT(buffer(0,1) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 42.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 42.0);

  ASSERT(constant_stream.getStart() == mu::kIndefinite);
  ASSERT(constant_stream.getEnd() == mu::kIndefinite);

  return 0;

}
