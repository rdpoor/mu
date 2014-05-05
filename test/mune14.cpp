/*
 * Experiment: when Ticks counter decreases, randomly pick
 * a new sound sample to play back.
 */
#include "multi_stream.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

/*
 * Maintain a collection of input streams.  Every time the tick counter
 * backs up, select a different input stream as the current stream.
 */
class ResetStream : public mu::MultiStream {
public:
  
  ResetStream() : prev_tick_ (mu::kIndefinite) { }
  ~ResetStream( void ) { }
  ResetStream& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) {
    if (sources_.size() == 0) {
      fprintf(stderr,"A");
      zero_buffer(buffer);
    } else {
      fprintf(stderr,"B");
      if (tick < prev_tick_) { reset(); }
      prev_tick_ = tick;
      current_stream_->step(buffer, tick, player);
    }
    return *this;
  }
  MultiStream& addSource(Stream *source) {
    return MultiStream::addSource(source);
  }

  mu::Tick getStart(){ return current_stream_->getStart(); }
  mu::Tick getEnd(){ return current_stream_->getEnd(); }

protected:
  ResetStream& reset();

  mu::Tick prev_tick_;
  mu::Stream *current_stream_;

};                            // class ResetStream

ResetStream& ResetStream::reset() {
  int index = std::rand() % sources_.size();
  fprintf(stderr, "reset: index=%d, size=%ld\n", index, sources_.size());
  current_stream_ = sources_.at(index);
  return *this;
}

mu::FileReadStream *getSound(std::string file_name) {
  return &(new mu::FileReadStream())->fileName(file_name).doNormalize(true);
}

int main() {
  ResetStream reset_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  reset_stream.addSource(getSound(SOUND_DIR "s00.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s01.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s02.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s03.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s04.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s05.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s06.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s07.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s08.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s09.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s10.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s11.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s12.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s13.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s14.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s15.wav"));
  reset_stream.addSource(getSound(SOUND_DIR "s16.wav"));

  loop_stream.setSource(&reset_stream).setLoopDuration(44100/3);
  player.setSource(&loop_stream);
  player.start();
  sleep(30);
  player.stop();

  return 0;
}
