/*
 * Create a class that emits a sequence of notes (or streams)
 */
#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

class SequenceStream : public mu::Stream {
public:
  
  SequenceStream() { }
  ~SequenceStream( void ) { }
  SequenceStream& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) {
    add_stream_.step(buffer, tick, player);
    return *this;
  }
  mu::Tick getStart(){ return add_stream_.getStart(); }
  mu::Tick getEnd(){ return add_stream_.getEnd(); }
  SequenceStream& addElement(mu::Stream& s, mu::Tick start, mu::Tick duration, double legato = 1.0);

protected:

  mu::AddStream add_stream_;

};                            // class SequenceStream

SequenceStream& SequenceStream::addElement(mu::Stream& src, 
                                           mu::Tick start, 
                                           mu::Tick duration, 
                                           double legato) {
  mu::CropStream *cs = &(new mu::CropStream())->setSource(&src).
    setStart(0).
    setEnd((mu::Tick)(duration * legato));
  mu::DelayStream *ds = &(new mu::DelayStream())->setSource(cs).setDelay(start);
  add_stream_.addSource(ds);

  return *this;
}

#define Q (44100/1.5)

mu::FileReadStream& snd02(std::string file_name) {
  return (new mu::FileReadStream())->fileName(file_name).doNormalize(true);
}

int main() {
  SequenceStream sequence_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  sequence_stream.addElement(snd02(SOUND_DIR "E4" ".wav"), 0.0*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "C5" ".wav"), 1.0*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "B4" ".wav"), 1.5*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "G4" ".wav"), 2.5*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "E4" ".wav"), 3.5*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "E4" ".wav"), 4.5*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "D4" ".wav"), 5.5*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "G4" ".wav"), 6.0*Q, 1.0*Q);
  sequence_stream.addElement(snd02(SOUND_DIR "E4" ".wav"), 6.5*Q, 1.5*Q);

  player.setSource(&sequence_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
