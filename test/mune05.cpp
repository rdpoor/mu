#include "mu.h"
#include "test_buzz.h"
#include "rt_player.h"

int main() {
  mu::RtPlayer player;       // allocate a real-time audio playback
  mu::TestBuzz buzz;         // allocate a phased pulse wave generator
  player.setSource(&buzz);   // connect output of generator to player.
  player.start();            // start streaming (should hear output)
  sleep(5);                  // ...for five seconds
  player.stop();             // then stop.
  return 0;
}
