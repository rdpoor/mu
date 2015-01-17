/*
 * Create a class that creates an arpeggiated chord.
 */
#include "add_sp.h"
#include "delay_sp.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"
#define SOUND_0 "A4"
#define SOUND_1 "C5"
#define SOUND_2 "E5"
#define SOUND_3 "F#5"


class ArpeggiateSP : public mu::SampleProcessor {
public:
  static const int kDefaultArpeggioDelay = 882;
  
  ArpeggiateSP() : arpeggio_delay_ (kDefaultArpeggioDelay) { }
  ~ArpeggiateSP( void ) { }
  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) {
    add_sp_.step(buffer, tick, true);
  }
  mu::Tick getStart(){ return add_sp_.getStart(); }
  mu::Tick getEnd(){ return add_sp_.getEnd(); }
  ArpeggiateSP& addFile(std::string file_name);
  mu::Tick getArpeggioDelay() { return arpeggio_delay_; }
  ArpeggiateSP& setArpeggioDelay(mu::Tick arpeggio_delay) {
    arpeggio_delay_ = arpeggio_delay;
    return *this;
  }
  

protected:

  mu::AddSP add_sp_;
  mu::Tick arpeggio_delay_;

};                            // class ArpeggiateSP

ArpeggiateSP& ArpeggiateSP::addFile(std::string file_name) {
  mu::FileReadSP *frs = &(new mu::FileReadSP())->fileName(file_name).doNormalize(true);
  long int n_notes = add_sp_.getSourceCount();
  fprintf(stderr, "dly_ = %ld, size() = %ld, tot=%ld\n",  
          arpeggio_delay_, 
          n_notes,
          arpeggio_delay_ * n_notes);
  mu::DelaySP *ds = &(new mu::DelaySP())->setSource(frs).setDelay(arpeggio_delay_ * n_notes);
  add_sp_.addSource(ds);

  return *this;
}


int main() {
  ArpeggiateSP arpeggiate_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  arpeggiate_sp.setArpeggioDelay(882);
  arpeggiate_sp.addFile(SOUND_DIR SOUND_0 ".wav");
  arpeggiate_sp.addFile(SOUND_DIR SOUND_1 ".wav");
  arpeggiate_sp.addFile(SOUND_DIR SOUND_2 ".wav");
  arpeggiate_sp.addFile(SOUND_DIR SOUND_3 ".wav");
  loop_sp.setSource(&arpeggiate_sp);
  loop_sp.setLoopDuration(arpeggiate_sp.getEnd()/2);
  player.setSource(&loop_sp);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
