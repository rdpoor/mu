/*
 * Test out SpliceSP to make an arpeggio, on the way to
 * a strummed instrument.
 *
 * See also mune12.cpp and mune17.cpp.
 *
 * For a strummed instrument, use one SpliceSP per string,
 * which guarantees that one string can only play one note at
 * a time.  Add all the strings together with an addSP
 * at the end.
 */
#include "delay_sp.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include "splice_sp.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (3306)

mu::SampleProcessor *getSoundFile(std::string file_name, mu::Tick delay) {
  mu::FileReadSP *frs = &((new mu::FileReadSP())->fileName(file_name).doNormalize(true));
  mu::DelaySP *ds = &((new mu::DelaySP())->setDelay(delay).setSource(frs));
  return ds;
}

int main() {
  mu::SpliceSP splice_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  splice_sp.addSource(getSoundFile(SOUND_DIR "A4" ".wav", 0.0*Q));
  splice_sp.addSource(getSoundFile(SOUND_DIR "C5" ".wav", 1.0*Q));
  splice_sp.addSource(getSoundFile(SOUND_DIR "E5" ".wav", 2.0*Q));
  splice_sp.addSource(getSoundFile(SOUND_DIR "F#5" ".wav", 3.0*Q));

  loop_sp.setSource(&splice_sp).setLoopDuration(splice_sp.getEnd());
  player.setSource(&loop_sp);
  player.start();
  fprintf(stderr, "Type [return] to quit:");
  getchar();
  player.stop();

  return 0;
}
