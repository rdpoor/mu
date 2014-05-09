/*
 * Unit Test ProbabilityStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "identity_stream.h"
#include "probability_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::ProbabilityStream probability_stream;
  mu::IdentityStream identity_stream;
  mu::CropStream crop_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  probability_stream.setSource(NULL).setProbability(1.0);
  ASSERT(probability_stream.getStart() == mu::kIndefinite);
  ASSERT(probability_stream.getEnd() == mu::kIndefinite);

  probability_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);

  probability_stream.setSource(&identity_stream).setProbability(0.0);
  ASSERT(probability_stream.getStart() == mu::kIndefinite);
  ASSERT(probability_stream.getEnd() == mu::kIndefinite);

  probability_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,1) == 0);

  probability_stream.setSource(&identity_stream).setProbability(1.0);
  ASSERT(probability_stream.getStart() == mu::kIndefinite);
  ASSERT(probability_stream.getEnd() == mu::kIndefinite);

  probability_stream.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  probability_stream.setSource(&identity_stream).setProbability(0.1);
#define N_TESTS 10000
  int c = 0;
  for (int i=0; i<N_TESTS; i++) {
    probability_stream.step(buffer, 0, player);
    if (buffer(1,0) != 0) c++;
  }
  ASSERT((c > N_TESTS*0.1*0.95) && (c < N_TESTS*0.1*1.05));

  probability_stream.setSource(&identity_stream).setProbability(0.9);
#define N_TESTS 10000
  c = 0;
  for (int i=0; i<N_TESTS; i++) {
    probability_stream.step(buffer, 0, player);
    if (buffer(1,0) != 0) c++;
  }
  ASSERT((c > N_TESTS*0.9*0.95) && (c < N_TESTS*0.9*1.05));

  // Assure that probability_stream start and end times are always
  // indefinite (since we don't know probability a priori)
  crop_stream.setSource(&identity_stream).setStart(30).setEnd(40);
  ASSERT(crop_stream.getStart() == 30);
  ASSERT(crop_stream.getEnd() == 40);
  probability_stream.setSource(&crop_stream).setProbability(1.0);
  ASSERT(probability_stream.getStart() == mu::kIndefinite);
  ASSERT(probability_stream.getEnd() == mu::kIndefinite);
  probability_stream.step(buffer, 0, player);
  ASSERT(probability_stream.getStart() == mu::kIndefinite);
  ASSERT(probability_stream.getEnd() == mu::kIndefinite);


}
