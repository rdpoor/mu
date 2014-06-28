/*
 * Together Now, Very Minor.  Click track.
 */
#include "click_track.h"

#define PERCUSSION_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

mu::SampleProcessor *make_click_track_sp() {
  mu::FileReadSP *thump_sp = new mu::FileReadSP();
  thump_sp->fileName(PERCUSSION_SOUND_DIR "s15.wav");

  mu::FileReadSP *tic_sp = new mu::FileReadSP();
  tic_sp->fileName(PERCUSSION_SOUND_DIR "s11.wav");

  mu::ConstantSP *tic_gain = new mu::ConstantSP();
  tic_gain->setValue(0.25);

  mu::MultiplySP *tic_attenuator = new mu::MultiplySP();
  tic_attenuator->addSource(tic_gain);
  tic_attenuator->addSource(tic_sp);

  mu::LoopSP *tic_loop = new mu::LoopSP();
  tic_loop->setSource(tic_attenuator);
  tic_loop->setLoopDuration(BEAT_TO_FRAME(1));
  tic_loop->setStart(0);
  tic_loop->setEnd(BEAT_TO_FRAME(4));

  mu::AddSP *mix_sp = new mu::AddSP();
  mix_sp->addSource(thump_sp);
  mix_sp->addSource(tic_loop);

  mu::LoopSP *loop_sp = new mu::LoopSP();
  loop_sp->setLoopDuration(MEASURE_TO_FRAME(1));
  loop_sp->setSource(mix_sp);
  loop_sp->setStart(MEASURE_TO_FRAME(M_INTRO));
  loop_sp->setEnd(MEASURE_TO_FRAME(M_END));

  return loop_sp;
}


