/*
 * Test FileReadStream and LoopStream
 */
#include "mu.h"
#include "rt_player.h"
#include "file_read_stream.h"
#include "loop_stream.h"

int main() {
  mu::RtPlayer player;      // allocate a real-time audio playback
  mu::LoopStream loop_stream;
  loop_stream.setLoopDuration(10784);
  mu::FileReadStream fr;
  fr.fileName("/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav").doNormalize(true);

  loop_stream.setSource(&fr);        // feed loop_stream from sound file
  player.setSource(&loop_stream); // feed real time playback from loop_stream
  player.start();            // start streaming (should hear output)
  sleep(5);                     // ...for five seconds
  player.stop();             // then stop.
  return 0;
}
