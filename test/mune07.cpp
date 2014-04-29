/*
 * Test FileReader and Looper
 */
#include "mu.h"
#include "rt_player.h"
#include "file_reader.h"
#include "looper.h"

int main() {
  mu::RtPlayer player;      // allocate a real-time audio playback
  mu::Looper looper;
  looper.setLoopDuration(11011);
  mu::FileReader fr;
  fr.fileName("/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav").doNormalize(true);

  looper.setSource(&fr);        // feed looper from sound file
  player.setSource(&looper); // feed real time playback from looper
  player.start();            // start streaming (should hear output)
  sleep(5);                     // ...for five seconds
  player.stop();             // then stop.
  return 0;
}
