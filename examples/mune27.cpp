/*
 * Test ResampleStream
 */

#include "crop_stream.h"
#include "file_read_stream.h"
#include "linseg_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "resample_stream.h"
#include "rt_player.h"

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  return frs;
}

int main() {
  mu::ResampleStream resample_stream;
  mu::LinsegStream linseg_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  linseg_stream.removeAllBreakpoints();
  linseg_stream.addBreakpoint(0, 0.0);
  linseg_stream.addBreakpoint(44100*1, 44100.0);
  linseg_stream.addBreakpoint(44100*2, 0.0);
  linseg_stream.addBreakpoint(44100*4, 44100*0.5);

  mu::Tick t0 = (44100*4)+1;

#if 0
  // more fun: play note at 25% pitch until a loop point,
  // then "seesaw" to extend the note for a while, then 
  // let it decay naturally.  Note: doing a splice like
  // this in an exponentially decaying string doesn't 
  // sound very good...

  double dpitch = 0.25;
  mu::Tick u0 = 0;              // start
  mu::Tick u1 = 7691;           // loop point 1
  mu::Tick u2 = 14221;          // loop point 2
  mu::Tick u3 = 51152;          // end of sound file

  linseg_stream.addBreakpoint(t0, u0);
  t0 += (u2-u0)/dpitch;
  linseg_stream.addBreakpoint(t0,u2);

  for (int i=0; i<5; i++) {
    t0 += (u2-u1)/dpitch;
    linseg_stream.addBreakpoint(t0,u1);
    t0 += (u2-u1)/dpitch;
    linseg_stream.addBreakpoint(t0,u2);
  }
  t0 += (u3-u2)/dpitch;
  linseg_stream.addBreakpoint(t0, u3);

#endif
  
  resample_stream.setSampleSource(getSoundFile(SOUND_DIR "A4" ".wav"));
  resample_stream.setTimingSource(&linseg_stream);

  loop_stream.setSource(&resample_stream).setLoopDuration(t0);

  player.setSource(&loop_stream);

  std::cout << resample_stream.inspect();

  player.start();
  fprintf(stderr, "Type [return] to quit:");  getchar();
  player.stop();

  return 0;
}
