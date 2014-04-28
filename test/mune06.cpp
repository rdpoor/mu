/*
 * Test FileReader
 */
#include "mu.h"
#include "transport.h"
#include "file_reader.h"

int main() {
  mu::Transport transport;      // allocate a real-time audio playback
  mu::FileReader fr;
  fr.fileName("/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav").doNormalize(true);

  transport.set_source(&fr);    // connect output of generator to transport.
  transport.start();            // start streaming (should hear output)
  sleep(5);                     // ...for five seconds
  transport.stop();             // then stop.
  return 0;
}
