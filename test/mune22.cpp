/*
 * Test out SpliceStream to make an arpeggio, on the way to
 * a strummed instrument.
 *
 * See also mune12.cpp and mune17.cpp.
 *
 * For a strummed instrument, use one SpliceStream per string,
 * which guarantees that one string can only play one note at
 * a time.  Add all the strings together with an addStream
 * at the end.
 */
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include "splice_stream.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (3306)

mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  return frs;
  // mu::CropStream *cs = &(new mu::CropStream())->setSource(frs).setStart(0);
  // return cs;
}

int main() {
  mu::SpliceStream splice_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  splice_stream.addSource(getSoundFile(SOUND_DIR "A4" ".wav"), 0.0*Q);
  splice_stream.addSource(getSoundFile(SOUND_DIR "C5" ".wav"), 1.0*Q);
  splice_stream.addSource(getSoundFile(SOUND_DIR "E5" ".wav"), 2.0*Q);
  splice_stream.addSource(getSoundFile(SOUND_DIR "F#5" ".wav"), 3.0*Q);

  loop_stream.setSource(&splice_stream).setLoopDuration(splice_stream.getEnd());
  player.setSource(&loop_stream);
  player.start();
  fprintf(stderr, "Type [return] to quit:");
  getchar();
  player.stop();

  return 0;
}
