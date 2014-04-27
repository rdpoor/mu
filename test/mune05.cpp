#include "mu.h"
#include "test_node.h"
#include "transport.h"

int main() {
  mu::Transport transport;      // allocate a real-time audio playback
  mu::TestNode gen;             // allocate a phased pulse wave generator
  transport.set_source(&gen);   // connect output of generator to transport.
  transport.start();            // start streaming (should hear output)
  sleep(5);                     // ...for five seconds
  transport.stop();             // then stop.
  return 0;
}
