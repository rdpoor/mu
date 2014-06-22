#include "mu.h"
#include "add_stream.h"
#include "rt_player.h"
#include "reverb_stream.h"
#include "constant_stream.h"
#include "multiply_stream.h"
#include "file_write_stream.h"

#include "bass.h"
#include "click_track.h"

int main() {
  mu::AddStream main_mix;
  mu::RtPlayer player;
  mu::ReverbStream reverb_stream;
  mu::ConstantStream master_gain;
  mu::MultiplyStream master_fader;
  mu::FileWriteStream file_write_stream;

  mu::Stream *bass_stream = make_bass_stream();
  mu::Stream *click_track_stream = make_click_track_stream();

  main_mix.addSource(bass_stream);
  main_mix.addSource(click_track_stream);

  master_gain.setValue(0.25);
  master_fader.addSource(&master_gain);
  master_fader.addSource(&main_mix);

  reverb_stream.setSource(&master_fader).setRatio(0.20);
  file_write_stream.setSource(&reverb_stream).
    setFileName("/Users/r/Projects/Mu/binary/tnvm.wav");
  player.setSource(&file_write_stream);

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();
  
  return 0;
}
