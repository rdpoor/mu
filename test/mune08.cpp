/*
 * Test TestStream, NrtPlayer, ValidatorStream
 */
#include "mu.h"
#include "nrt_player.h"
#include "test_stream.h"
#include "validator_stream.h"
#include <unistd.h>

bool validate(mu::Tick tick, stk::StkFrames& buffer, long int frame_index) {
  mu::Tick expected = tick;
  for (int chan=0; chan<buffer.channels(); chan++) {
    mu::Tick got = (mu::Tick)round(buffer(frame_index, chan));
    if (expected != got) {
      fprintf(stderr, "at tick %ld, got %ld, expected %ld\n", tick, got, expected);
      // if we had a handle onto player, we could stop it now
      return false;
    }
  }
  return true;
}

int main() {
  mu::TestStream test_stream;
  mu::ValidatorStream validator;
  mu::NrtPlayer player;

  validator.setCallback(validate);
  validator.setSource(&test_stream);
  player.setSource(&validator);

  player.start();
  sleep(10);
  player.stop();

  return 0;
}
