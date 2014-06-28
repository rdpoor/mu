#include "tnvm.h"

#include "bass.h"
#include "click_track.h"
#include "percussion.h"

int main() {
  mu::AddSP main_mix;
  mu::RtPlayer player;
  mu::ReverbSP reverb_sp;
  mu::ConstantSP master_gain;
  mu::MultiplySP master_fader;
  mu::FileWriteSP file_write_sp;

  main_mix.addSource(make_bass_sp());
  main_mix.addSource(make_click_track_sp());
  main_mix.addSource(make_percussion_sp());

  master_gain.setValue(0.25);
  master_fader.addSource(&master_gain);
  master_fader.addSource(&main_mix);

  reverb_sp.setSource(&master_fader).setRatio(0.20);
  file_write_sp.setSource(&reverb_sp).
    setFileName("/Users/r/Projects/Mu/binary/tnvm.wav");
  player.setSource(&file_write_sp);

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();
  
  return 0;
}
