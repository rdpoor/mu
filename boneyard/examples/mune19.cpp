/*
 * Fade in/fade out to test LinsegSP and MultiplySP.
 */

#include "file_read_sp.h"
#include "linseg_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "multiply_sp.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_FILE "/Users/r/Projects/Musics/TNVM/sources/Chords/Bb.wav"

#define LOOP_DURATION (44100/2)

int main() {
  mu::FileReadSP file_read_sp;
  mu::LinsegSP linseg_sp;
  mu::MultiplySP multiply_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  file_read_sp.fileName(SOUND_FILE).doNormalize(true);

  loop_sp.setSource(&file_read_sp).setLoopDuration(LOOP_DURATION);

  linseg_sp.addBreakpoint(44100*0,0.0);
  linseg_sp.addBreakpoint(44100*4,1.0);
  linseg_sp.addBreakpoint(44100*8,0.0);

  multiply_sp.addSource(&loop_sp);
  multiply_sp.addSource(&linseg_sp);

  player.setSource(&multiply_sp);

  player.start();
  sleep(8.0);
  player.stop();

  return 0;
}
