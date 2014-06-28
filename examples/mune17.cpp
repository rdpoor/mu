/*
 * Doh! SequenceSP is also just right for making arpeggios...
 * (See also mune12.cpp)
 */
#include "sequence_sp.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (882)

mu::SampleProcessor *getSoundFile(std::string file_name) {
  mu::FileReadSP *frs = &((new mu::FileReadSP())->fileName(file_name).doNormalize(true));
  return frs;
}

int main() {
  mu::SequenceSP sequence_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  sequence_sp.addSource(getSoundFile(SOUND_DIR "A4" ".wav"), 0.0*Q);
  sequence_sp.addSource(getSoundFile(SOUND_DIR "C5" ".wav"), 1.0*Q);
  sequence_sp.addSource(getSoundFile(SOUND_DIR "E5" ".wav"), 2.0*Q);
  sequence_sp.addSource(getSoundFile(SOUND_DIR "F#5" ".wav"), 3.0*Q);

  loop_sp.setSource(&sequence_sp).setLoopDuration(sequence_sp.getEnd());
  player.setSource(&loop_sp);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
