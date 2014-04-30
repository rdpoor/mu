/*
 * Test IdentityStream, NrtPlayer, ValidatorStream
 */
#include "mu.h"
#include "nrt_player.h"
#include "identity_stream.h"
#include "map_stream.h"
#include <unistd.h>

void validate(stk::StkFrames& buffer, mu::Tick tick, mu::Player& player, mu::Tick frame_index) {
  if (tick == 44100) {
    printf("stopping playback at tick=%ld\n", tick);
    // NB: this is a *request* to stop.  player will finish out the
    // current buffer before stopping.
    player.stop();
  }
  mu::Tick expected = tick;
  for (int chan=0; chan<buffer.channels(); chan++) {
    mu::Tick got = (mu::Tick)round(buffer(frame_index, chan));
    if (expected != got) {
      fprintf(stderr, "at tick %ld, got %ld, expected %ld\n", tick, got, expected);
      player.stop(true);        // stop immediately.
      return;
    }
  }
}

int main() {
  mu::IdentityStream identity_stream;
  mu::MapStream map_stream;
  mu::NrtPlayer player;

  map_stream.setCallback(validate);
  map_stream.setSource(&identity_stream);
  player.setSource(&map_stream);

  player.start();
  sleep(10);
  player.stop();

  return 0;
}
