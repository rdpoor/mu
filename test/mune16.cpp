/*
 * Test SequenceStream
 */
#include "sequence_stream.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define Q (44100/1.5)

mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  mu::CropStream *cs = &(new mu::CropStream())->setSource(frs).setStart(0);
  return cs;
}

int main() {
  mu::SequenceStream sequence_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  sequence_stream.addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 0.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "C5" ".wav"), 1.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "B4" ".wav"), 1.5*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "G4" ".wav"), 2.5*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 3.5*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 4.5*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "D4" ".wav"), 5.5*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "G4" ".wav"), 6.0*Q);
  sequence_stream.addSource(getSoundFile(SOUND_DIR "E4" ".wav"), 6.5*Q);

  loop_stream.setSource(&sequence_stream).setLoopDuration(sequence_stream.getEnd());
  player.setSource(&loop_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
