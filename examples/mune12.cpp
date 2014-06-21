/*
 * Create a class that creates an arpeggiated chord.
 */
#include "add_stream.h"
#include "delay_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"
#define SOUND_0 "A4"
#define SOUND_1 "C5"
#define SOUND_2 "E5"
#define SOUND_3 "F#5"


class ArpeggiateStream : public mu::Stream {
public:
  static const int kDefaultArpeggioDelay = 882;
  
  ArpeggiateStream() : arpeggio_delay_ (kDefaultArpeggioDelay) { }
  ~ArpeggiateStream( void ) { }
  void step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) {
    add_stream_.step(buffer, tick, player);
  }
  mu::Tick getStart(){ return add_stream_.getStart(); }
  mu::Tick getEnd(){ return add_stream_.getEnd(); }
  ArpeggiateStream& addFile(std::string file_name);
  mu::Tick getArpeggioDelay() { return arpeggio_delay_; }
  ArpeggiateStream& setArpeggioDelay(mu::Tick arpeggio_delay) {
    arpeggio_delay_ = arpeggio_delay;
    return *this;
  }
  

protected:

  mu::AddStream add_stream_;
  mu::Tick arpeggio_delay_;

};                            // class ArpeggiateStream

ArpeggiateStream& ArpeggiateStream::addFile(std::string file_name) {
  mu::FileReadStream *frs = &(new mu::FileReadStream())->fileName(file_name).doNormalize(true);
  long int n_notes = add_stream_.getSourceCount();
  fprintf(stderr, "dly_ = %ld, size() = %ld, tot=%ld\n",  
          arpeggio_delay_, 
          n_notes,
          arpeggio_delay_ * n_notes);
  mu::DelayStream *ds = &(new mu::DelayStream())->setSource(frs).setDelay(arpeggio_delay_ * n_notes);
  add_stream_.addSource(ds);

  return *this;
}


int main() {
  ArpeggiateStream arpeggiate_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  arpeggiate_stream.setArpeggioDelay(882);
  arpeggiate_stream.addFile(SOUND_DIR SOUND_0 ".wav");
  arpeggiate_stream.addFile(SOUND_DIR SOUND_1 ".wav");
  arpeggiate_stream.addFile(SOUND_DIR SOUND_2 ".wav");
  arpeggiate_stream.addFile(SOUND_DIR SOUND_3 ".wav");
  loop_stream.setSource(&arpeggiate_stream);
  loop_stream.setLoopDuration(arpeggiate_stream.getEnd()/2);
  player.setSource(&loop_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
