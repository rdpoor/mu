/*
 * Doh! SequenceStream is also just right for making arpeggios...
 * (See also mune12.cpp)
 */
#include "sequence_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (882)

mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  return frs;
}

int main() {
  mu::SequenceStream sequence_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  sequence_stream.addSource(getSoundFile(SOUND_DIR "A4" ".wav"), 0.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "C5" ".wav"), 1.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "E5" ".wav"), 2.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "F#5" ".wav"), 3.0*Q);

  loop_stream.setSource(&sequence_stream).setLoopDuration(sequence_stream.getEnd());
  player.setSource(&loop_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
