/*
 * Identical to mune14, but uses library class RandomSelectSP.
 */
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "random_select_sp.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

mu::FileReadSP *getSound(std::string file_name) {
  return &(new mu::FileReadSP())->fileName(file_name).doNormalize(true);
}

int main() {
  mu::RandomSelectSP random_select_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  random_select_sp.addSource(getSound(SOUND_DIR "s00.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s01.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s02.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s03.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s04.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s05.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s06.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s07.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s08.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s09.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s10.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s11.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s12.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s13.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s14.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s15.wav"));
  random_select_sp.addSource(getSound(SOUND_DIR "s16.wav"));

  loop_sp.setSource(&random_select_sp).setLoopDuration(44100/3);
  player.setSource(&loop_sp);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
