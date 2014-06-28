/*
 * Unit Test ProbabilitySP
 */
#include "assert.h"
#include "crop_sp.h"
#include "identity_sp.h"
#include "probability_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::ProbabilitySP probability_sp;
  mu::IdentitySP identity_sp;
  mu::CropSP crop_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  probability_sp.setSource(NULL).setProbability(1.0);
  ASSERT(probability_sp.getStart() == mu::kIndefinite);
  ASSERT(probability_sp.getEnd() == mu::kIndefinite);

  probability_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);

  probability_sp.setSource(&identity_sp).setProbability(0.0);
  ASSERT(probability_sp.getStart() == mu::kIndefinite);
  ASSERT(probability_sp.getEnd() == mu::kIndefinite);

  probability_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == 0);
  ASSERT(buffer(FRAME_COUNT-1,1) == 0);

  probability_sp.setSource(&identity_sp).setProbability(1.0);
  ASSERT(probability_sp.getStart() == mu::kIndefinite);
  ASSERT(probability_sp.getEnd() == mu::kIndefinite);

  probability_sp.step(buffer, 0, player);
  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer(FRAME_COUNT-1,0) == FRAME_COUNT-1);
  ASSERT(buffer(FRAME_COUNT-1,1) == FRAME_COUNT-1);

  probability_sp.setSource(&identity_sp).setProbability(0.1);
#define N_TESTS 10000
  int c = 0;
  for (int i=0; i<N_TESTS; i++) {
    probability_sp.step(buffer, 0, player);
    if (buffer(1,0) != 0) c++;
  }
  ASSERT((c > N_TESTS*0.1*0.95) && (c < N_TESTS*0.1*1.05));

  probability_sp.setSource(&identity_sp).setProbability(0.9);
#define N_TESTS 10000
  c = 0;
  for (int i=0; i<N_TESTS; i++) {
    probability_sp.step(buffer, 0, player);
    if (buffer(1,0) != 0) c++;
  }
  ASSERT((c > N_TESTS*0.9*0.95) && (c < N_TESTS*0.9*1.05));

  // Assure that probability_sp start and end times are always
  // indefinite (since we don't know probability a priori)
  crop_sp.setSource(&identity_sp).setStart(30).setEnd(40);
  ASSERT(crop_sp.getStart() == 30);
  ASSERT(crop_sp.getEnd() == 40);
  probability_sp.setSource(&crop_sp).setProbability(1.0);
  ASSERT(probability_sp.getStart() == mu::kIndefinite);
  ASSERT(probability_sp.getEnd() == mu::kIndefinite);
  probability_sp.step(buffer, 0, player);
  ASSERT(probability_sp.getStart() == mu::kIndefinite);
  ASSERT(probability_sp.getEnd() == mu::kIndefinite);


}
