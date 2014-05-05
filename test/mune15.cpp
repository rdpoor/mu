/*
 * Identical to mune14, but uses library class RandomSelectStream.
 */
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "random_select_stream.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

mu::FileReadStream *getSound(std::string file_name) {
  return &(new mu::FileReadStream())->fileName(file_name).doNormalize(true);
}

int main() {
  mu::RandomSelectStream random_select_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  random_select_stream.addSource(getSound(SOUND_DIR "s00.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s01.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s02.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s03.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s04.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s05.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s06.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s07.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s08.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s09.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s10.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s11.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s12.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s13.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s14.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s15.wav"));
  random_select_stream.addSource(getSound(SOUND_DIR "s16.wav"));

  loop_stream.setSource(&random_select_stream).setLoopDuration(44100/3);
  player.setSource(&loop_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
