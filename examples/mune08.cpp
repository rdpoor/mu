/*
 * Test IdentitySP, NrtPlayer, ValidatorSP
 */
#include "mu.h"
#include "nrt_player.h"
#include "identity_sp.h"
#include "map_sp.h"
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
  mu::IdentitySP identity_sp;
  mu::MapSP map_sp;
  mu::NrtPlayer player;

  map_sp.setCallback(validate);
  map_sp.setSource(&identity_sp);
  player.setSource(&map_sp);

  player.start();
  sleep(10);
  player.stop();

  return 0;
}
