/*
 * Create a class that emits a sequence of notes (or streams)
 */
#include "add_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

class SequenceSP : public mu::SampleProcessor {
public:
  
  SequenceSP() { }
  ~SequenceSP( void ) { }
  void step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) {
    add_sp_.step(buffer, tick, player);
  }
  mu::Tick getStart(){ return add_sp_.getStart(); }
  mu::Tick getEnd(){ return add_sp_.getEnd(); }
  SequenceSP& addElement(mu::SampleProcessor& s, mu::Tick start, mu::Tick duration, double legato = 1.0);

protected:

  mu::AddSP add_sp_;

};                            // class SequenceSP

SequenceSP& SequenceSP::addElement(mu::SampleProcessor& src, 
                                           mu::Tick start, 
                                           mu::Tick duration, 
                                           double legato) {
  mu::CropSP *cs = &(new mu::CropSP())->setSource(&src).
    setStart(0).
    setEnd((mu::Tick)(duration * legato));
  mu::DelaySP *ds = &(new mu::DelaySP())->setSource(cs).setDelay(start);
  add_sp_.addSource(ds);

  return *this;
}

#define Q (44100/1.5)

mu::FileReadSP& snd02(std::string file_name) {
  return (new mu::FileReadSP())->fileName(file_name).doNormalize(true);
}

int main() {
  SequenceSP sequence_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  sequence_sp.addElement(snd02(SOUND_DIR "E4" ".wav"), 0.0*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "C5" ".wav"), 1.0*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "B4" ".wav"), 1.5*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "G4" ".wav"), 2.5*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "E4" ".wav"), 3.5*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "E4" ".wav"), 4.5*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "D4" ".wav"), 5.5*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "G4" ".wav"), 6.0*Q, 1.0*Q);
  sequence_sp.addElement(snd02(SOUND_DIR "E4" ".wav"), 6.5*Q, 1.5*Q);

  player.setSource(&sequence_sp);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
