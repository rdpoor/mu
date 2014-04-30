/*
 * Test Looper using TestStream and MapStream
 */
#include "looper.h"
#include "map_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "test_stream.h"
#include <unistd.h>

#define LOOP_INTERVAL (1000)

void validate(stk::StkFrames& buffer, mu::Tick tick, mu::Player& player, mu::Tick frame_index) {
  if (tick == 4410) {
    printf("stopping playback at tick=%ld\n", tick);
    // NB: this is a *request* to stop.  player will finish out the
    // current buffer before stopping.
    player.stop();
  }
  mu::Tick expected = tick % LOOP_INTERVAL;
  for (int chan=0; chan<buffer.channels(); chan++) {
    mu::Tick got = (mu::Tick)round(buffer(frame_index, chan));
    if (expected != got) {
      fprintf(stderr, "at tick %ld, got %ld, expected %ld\n", tick, got, expected);
      // player.stop(true);        // stop immediately.
      return;
    }
  }
}

int main() {
  mu::TestStream test_stream;
  mu::Looper looper;
  mu::MapStream map_stream;
  mu::NrtPlayer player;

  looper.setSource(&test_stream).setLoopDuration(LOOP_INTERVAL);
  map_stream.setSource(&looper).setCallback(validate);
  player.setSource(&map_stream);

  player.start();
  sleep(5);
  player.stop();

  return 0;
}
