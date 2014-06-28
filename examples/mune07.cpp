/*
 * Test FileReadSP and LoopSP
 */
#include "mu.h"
#include "rt_player.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include <unistd.h>

int main() {
  mu::RtPlayer player;      // allocate a real-time audio playback
  mu::LoopSP loop_sp;
  loop_sp.setLoopDuration(10784);
  mu::FileReadSP fr;
  fr.fileName("/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav").doNormalize(true);

  fprintf(stderr,"A");
  loop_sp.setSource(&fr);        // feed loop_sp from sound file
  fprintf(stderr,"B");
  player.setSource(&loop_sp); // feed real time playback from loop_sp
  fprintf(stderr,"C");
  player.start();            // start streaming (should hear output)
  fprintf(stderr,"D");
  fprintf(stderr,"loop_sp = %p\n", &loop_sp);
  sleep(5);                     // ...for five seconds
  player.stop();             // then stop.
  return 0;
}
