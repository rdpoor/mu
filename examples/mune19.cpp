/*
 * Fade in/fade out to test LinsegStream and MultiplyStream.
 */

#include "file_read_stream.h"
#include "linseg_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "multiply_stream.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_FILE "/Users/r/Projects/Musics/TNVM/sources/Chords/Bb.wav"

#define LOOP_DURATION (44100/2)

int main() {
  mu::FileReadStream file_read_stream;
  mu::LinsegStream linseg_stream;
  mu::MultiplyStream multiply_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  file_read_stream.fileName(SOUND_FILE).doNormalize(true);

  loop_stream.setSource(&file_read_stream).setLoopDuration(LOOP_DURATION);

  linseg_stream.addBreakpoint(44100*0,0.0);
  linseg_stream.addBreakpoint(44100*4,1.0);
  linseg_stream.addBreakpoint(44100*8,0.0);

  multiply_stream.addSource(&loop_stream);
  multiply_stream.addSource(&linseg_stream);

  player.setSource(&multiply_stream);

  player.start();
  sleep(8.0);
  player.stop();

  return 0;
}
