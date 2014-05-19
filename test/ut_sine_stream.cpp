/*
 * Unit Test SineStream
 */
#include "assert.h"
#include "mu.h"
#include "sine_stream.h"
#include "nrt_player.h"
#include <unistd.h>
#include <math.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::SineStream sine_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  sine_stream.setFrequency(440.0);
  sine_stream.setPhase(0.0);
  sine_stream.setAmplitude(1.0);

  sine_stream.step(buffer, 0, player);

  for (long int i=0; i<buffer.frames(); i++) {
    double t = (double)i / buffer.dataRate();
    double expect = 1.0 * sin(t * 2.0 * M_PI * 440.0 + 0.0);
    for (int j=0; j<buffer.channels(); j++) {
      ASSERT(near(buffer(i,j), expect) == true);
    }
  }

  ASSERT(sine_stream.getStart() == mu::kIndefinite);
  ASSERT(sine_stream.getEnd() == mu::kIndefinite);

  sine_stream.setFrequency(440.0);
  ASSERT(near(sine_stream.getFrequency(), 440.0) == true);
  ASSERT(near(sine_stream.getPitch(), 69.0) == true);

  sine_stream.setPitch(48);
  ASSERT(near(sine_stream.getFrequency(), 130.8127827) == true);
  ASSERT(near(sine_stream.getPitch(), 48.0) == true);
  
  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
