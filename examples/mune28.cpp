/*
 * Sketch for Flight of the Bumble Bee.
 *
 * I need to decide if (and how) to score this.  Fow now, I'll
 * just use a compact C++ notation to test a few bars.
 */
#include "mu.h"

#include "NRev.h"
#include "add_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "fade_sp.h"
#include "file_read_sp.h"
#include "file_write_sp.h"
#include "linseg_sp.h"
#include "multiply_sp.h"
#include "null_sp.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

// ================================================================
/*
 * SPSet maps between a pitch number (i.e. midi pitch) and
 * a stream that produces that pitch.  A normal example is mapping
 * a pitch to a FileReadSP().
 */
class SPSet {
public:
  SPSet( void ) {
  }
  ~SPSet( void ) {
  }
  virtual mu::SampleProcessor *findSP(std::string name) = 0;

protected:
  mu::NullSP null_sp_;
};                              // class SPSet

// ================================================================
class FileReadSPSet : public SPSet {
public:
  mu::SampleProcessor *findSP(std::string name) {
    if (cache_.find(name) == cache_.end()) {
      std::string file_name = makeFileName(name);
      mu::FileReadSP *frs = new mu::FileReadSP();
      frs->fileName(file_name).doNormalize(true);
      cache_[name] = frs;
    }
    return cache_[name];
  }

  std::string getDirectoryName( void ) { return directory_name_; }
  FileReadSPSet& setDirectoryName(std::string directory_name) { 
    directory_name_ = directory_name; 
    return *this;
  }

protected:
  std::string makeFileName(std::string name) {
    std::stringstream ss;
    ss << directory_name_ << name << ".wav";
    return ss.str();
  }
  std::string directory_name_;
  std::map<std::string, mu::FileReadSP *> cache_;
};                              // class FileReadSPSet

// ================================================================

/*
 * Generate notes, chords and rests
 */
class Hand {
public:
  // generate a rest by incrementing time by duration.
  Hand& rest(double duration) {
    time_ += duration;
    return *this;
  }

  // generate a note and increment time by duration.
  Hand& note(double duration, std::string pitch, double legato=1.5) {
    make_note(time_, duration, pitch, legato);
    time_ += duration;
    return *this;
  }

  // play up to 5 notes as a chord.  arpeggio rolls the chord as a
  // factor of the duration: 0 = no roll, 1.0 = roll over duration.
  Hand& chord(double duration, 
              std::string n0 = "",
              std::string n1 = "",
              std::string n2 = "",
              std::string n3 = "",
              std::string n4 = "",
              double legato = 1.0,
              double arpeggio = 0.0) {
    make_note(time_ + 0.0 * arpeggio * duration, duration, n0, legato);
    make_note(time_ + 1.0 * arpeggio * duration, duration, n1, legato);
    make_note(time_ + 2.0 * arpeggio * duration, duration, n2, legato);
    make_note(time_ + 3.0 * arpeggio * duration, duration, n3, legato);
    make_note(time_ + 4.0 * arpeggio * duration, duration, n4, legato);
    time_ += duration;
    return *this;
  }

  double getTime( void ) { return time_; }
  Hand& setTime(double time) { time_ = time; return *this; }

  SPSet *getSPSet( void ) { return stream_set_; }
  Hand& setSPSet(SPSet *stream_set) { stream_set_ = stream_set; return *this; }

  mu::SampleProcessor *getSP( void ) { return &add_sp_; }

protected:
  // add a note to the stream.  Does not alter time_.
  Hand& make_note(double time, double duration, std::string pitch, double legato) {
    if (pitch.empty()) return *this;

    mu::SampleProcessor *s = stream_set_->findSP(pitch);
    mu::CropSP *cs = &(new mu::CropSP())->
      setStart(0).
      setEnd((mu::Tick)(duration * legato)).
      setSource(s);
    mu::DelaySP *ds = &(new mu::DelaySP())->
      setDelay((mu::Tick)time).
      setSource(cs);
    mu::FadeSP *fs = &(new mu::FadeSP())->
      setFadeTime(100).
      setSource(ds);
    add_sp_.addSource(fs);
    return *this;
  }

  double time_;
  mu::AddSP add_sp_;
  SPSet *stream_set_;
};                              // class Hand

// ================================================================
/*
 * Generate dynamics
 */
class Dynamics {
public:

  Dynamics( void ) :
    time_ ( 0.0 ),
    value_ ( 0.0 ),
    is_ramping_ ( true ) {
    multiply_sp_.addSource(&linseg_sp_);
  }
    
  // generate a rest by incrementing time by duration.
  Dynamics& rest(double duration) {
    time_ += duration;
    return *this;
  }

  // finish previous segment, start a constant segment
  Dynamics& set(double duration, double value) {
    return emit_segment(duration, value, false);
  }

  // finish previous segment, start a ramping segment
  Dynamics& ramp(double duration, double value) {
    return emit_segment(duration, value, true);
  }

  double getTime( void ) { return time_; }
  Dynamics& setTime(double time) { time_ = time; return *this; }

  mu::MultiplySP *getSP( void ) { return &multiply_sp_; }

protected:
  Dynamics& emit_segment( double duration, double value, bool start_ramping ) {
    if (!is_ramping_) {
      // finish previous segment at current time - epsilon
      // and previous value.
      linseg_sp_.addBreakpoint((mu::Tick)time_ - 1, value_);
    }
    is_ramping_ = start_ramping;
    linseg_sp_.addBreakpoint((mu::Tick)time_, value);
    value_ = value;
    time_ += duration;
    return *this;
  }

  double time_;
  double value_;
  bool is_ramping_;
  mu::MultiplySP multiply_sp_;
  mu::LinsegSP linseg_sp_;

};                              // class Dynamics

// ================================================================

class ReverbSP : public mu::SampleProcessor {
private:
  void setup() {
    if (reverb_ == NULL) {
      reverb_ = new stk::NRev();
      reverb_->setT60( 2.0 );
    }
    return;
  }

public:

  ReverbSP( void ) {
    setRatio(1.0);
  }

  ~ReverbSP( void ) {
    delete reverb_;
    reverb_ = NULL;
  }

  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) {
    if (source_ == NULL) {
      zeroBuffer (buffer);
    } else {
      setup();
      int frame_count = buffer.frames();
      int channel_count = buffer.channels();
      buffer_.resize(frame_count, channel_count);

      source_->step(buffer_, tick, true);
      for (int frame=0; frame<frame_count; frame++) {
        stk::StkFloat x = buffer_(frame, 0); // input from channel 0 only
        reverb_->tick(x);
        for (int channel=0; channel<channel_count; channel++) {
          buffer(frame, channel) = (dry_ * x) + (wet_ * reverb_->lastOut(channel));
        }
      }
    }
  }

  mu::Tick getEnd() {
    if ((source_ == NULL) || (source_->getEnd() == mu::TickUtils::indefinite())) {
      return mu::TickUtils::indefinite();
    } else {
      // badly written to extend end time by reverb tail
      return source_->getEnd() + 44100 * 2.5;
    }
  }

  mu::SampleProcessor *getSource() { return source_; }
  ReverbSP& setSource(mu::SampleProcessor *source) { source_ = source; return *this; }

  stk::StkFloat getRatio( void ) { return wet_; }
  ReverbSP& setRatio(stk::StkFloat ratio) { 
    wet_ = ratio;
    dry_ = 1.0 - ratio;
    return *this;
  }
  
protected:
  mu::SampleProcessor *source_;
  stk::StkFrames buffer_;
  stk::StkFloat dry_;
  stk::StkFloat wet_;
  stk::NRev *reverb_;
}; // class ReverbSP
    

// ================================================================

int main() {
  FileReadSPSet stream_set;
  Hand right_hand;
  Hand left_hand;
  Dynamics dynamics;
  ReverbSP reverb_sp;
  mu::NullSP null_sp;   // should not need?
  mu::AddSP add_sp;
  mu::FileWriteSP file_write_sp;
  mu::RtPlayer player;

  stream_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  right_hand.setSPSet(&stream_set);
  left_hand.setSPSet(&stream_set);
  
  // mix left and right hands into one stream
  add_sp.addSource(right_hand.getSP());
  add_sp.addSource(left_hand.getSP());
  add_sp.addSource(&null_sp); // avoid click at end?
  
  // add dynamics
  dynamics.getSP()->addSource(&add_sp);

  // add reverb
  reverb_sp.setSource(dynamics.getSP());
  reverb_sp.setRatio(0.25);

  // write and play the result
  file_write_sp.setSource(&reverb_sp).setFileName("/Users/r/Projects/Mu/Scores/fotu.wav");
  player.setSource(&file_write_sp);

  // create some handy dynamics
  double global_gain = 0.33;

  // double dyn_ppp = global_gain*16/128.0;
  double dyn_pp = global_gain*32/128.0;
  double dyn_p = global_gain*48/128.0;
  double dyn_mp = global_gain*64/128.0;
  double dyn_mf = global_gain*80/128.0;
  double dyn_f = global_gain*96/128.0;
  // double dyn_ff = global_gain*112/128.0;
  // double dyn_fff = global_gain*127/128.0;

  // create some handy durations
  double dur_q = (44100 * 0.37); // quarter
  double dur_e = dur_q / 2.0;    // eigth
  double dur_s = dur_q / 4.0;    // sixteenth
  double dur_h = dur_q * 2.0;    // half (1 measure)
  double dur_w = dur_q * 4.0;    // whole

  // and some useful symbolic names
  double staccato=0.5;
  double legato=1.5;

  double rolled_chord=0.2;
  double small_arpeggio=0.05;

  // here is the score...
  right_hand.setTime(0);
  left_hand.setTime(0);
  dynamics.setTime(0);

  // a little intro...
  dynamics.set(dur_h*8, dyn_p);
  right_hand.note(dur_h, "e5", 0.8);
  right_hand.note(dur_h, "c5", 0.8);
  right_hand.note(dur_h, "a4", 0.8);
  right_hand.note(dur_h, "e4", 0.8);
  right_hand.note(dur_h, "a4", 0.8);
  right_hand.note(dur_h, "c5", 0.8);
  right_hand.note(dur_w, "e5", 0.8);
  left_hand.rest(dur_h*8);

  right_hand.rest(dur_h);
  left_hand.rest(dur_h);
  dynamics.rest(dur_h);

  // p1.1
  printf("page 1.1 lh time = %f\n", left_hand.getTime());
  printf("page 1.1 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.set(dur_h, dyn_f);
  left_hand.chord(dur_e, "e3", "b3", "gs4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);

  dynamics.ramp(dur_h, dyn_f);
  left_hand.chord(dur_e, "e2", "b2", "gs3", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);

  // p1.2
  printf("page 1.2 lh time = %f\n", left_hand.getTime());
  printf("page 1.2 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "b3", legato);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "b3", legato);

  dynamics.set(dur_h, dyn_pp);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ds4", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "c4");
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a3");
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "ds4", legato);

  // p1.3
  printf("page 1.3 lh time = %f\n", left_hand.getTime());
  printf("page 1.3 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ds4", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "c4");
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a3");
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "ds4", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "b3", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "e3", "a3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "e3", "gs3", "b3", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "ds4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ds4", legato);

  // p1.4
  printf("page 1.4 lh time = %f\n", left_hand.getTime());
  printf("page 1.4 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "c3", "e3", "a3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "ef4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "b3", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "e3", "a3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "e3", "gs3", "b3", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "ds4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "gs4", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "cs3", "g3", "a3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "f3", "a3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "b3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "gs4", legato);

  // p2.1
  printf("page 2.1 lh time = %f\n", left_hand.getTime());
  printf("page 2.1 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  
  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "b3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "gs4", legato);
  
  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "g3", "bf3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "e4", legato);
  
  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "e3", "g3", "a3", "cs4", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  
  // p2.2
  printf("page 2.2 lh time = %f\n", left_hand.getTime());
  printf("page 2.2 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "bf3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "af4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "e4", legato);
  
  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "d3", "f3", "a3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "cs3", "e3", "g3", "a3", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  
  dynamics.rest(dur_h);
  left_hand.chord(dur_s, "d3", "f3", "", "", "", 2.0, small_arpeggio); // 2x legato
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  right_hand.note(dur_e, "a4", legato);
  right_hand.rest(dur_e);
  right_hand.note(dur_q, "a4", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  right_hand.note(dur_h, "bf4", legato);
  
  // p2.3
  printf("page 2.3 lh time = %f\n", left_hand.getTime());
  printf("page 2.3 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.rest(dur_s);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "b3", legato);
  left_hand.note(dur_s, "a3", legato);
  right_hand.note(dur_q, "a4", legato);
  right_hand.note(dur_q, "a4", legato);
  
  dynamics.rest(dur_h);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  left_hand.note(dur_s, "bf3", legato);
  left_hand.note(dur_s, "af3", legato);
  right_hand.note(dur_h, "bf4", legato);
  
  dynamics.rest(dur_h);
  left_hand.note(dur_e, "bf3");
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "af3", "bf3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);

  dynamics.ramp(dur_h, dyn_pp);
  left_hand.chord(dur_e, "af3", "", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "af3", "bf3", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "g3", "b3", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "fs3", "c4", "", "", "", staccato, small_arpeggio);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  
  // p2.4
  printf("page 2.4 lh time = %f\n", left_hand.getTime());
  printf("page 2.4 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.set(dur_h, dyn_mf);
  left_hand.chord(dur_h * 2, "f3", "cs4", "", "", "", staccato, small_arpeggio);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);

  dynamics.ramp(dur_h, dyn_mf);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);

  dynamics.ramp(dur_h, dyn_pp);
  left_hand.note(dur_s, "fs3", 2.0); // 2x legato
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_e, "a4", legato);
  right_hand.rest(dur_e);
  right_hand.note(dur_q, "d4", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  right_hand.chord(dur_h, "df5", "ef5", "", "", "", legato, small_arpeggio);

  
  // p2.5
  printf("page 2.5 lh time = %f\n", left_hand.getTime());
  printf("page 2.5 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.rest(dur_s);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_q, "d5", legato);
  right_hand.note(dur_q, "d5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "df4", legato);
  right_hand.chord(dur_h, "df5", "ef5", "", "", "", legato, small_arpeggio);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "d4");
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "df4", "ef4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);

  dynamics.ramp(dur_h, dyn_pp);
  left_hand.chord(dur_e, "d4", "", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "df4", "ef4", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "c4", "e4", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "b3", "f4", "", "", "", staccato, small_arpeggio);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  
  // p2.6
  printf("page 2.6 lh time = %f\n", left_hand.getTime());
  printf("page 2.6 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.set(dur_h, dyn_mf);
  left_hand.chord(dur_h * 2, "bf3", "fs4", "", "", "", staccato, small_arpeggio);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);

  dynamics.ramp(dur_h, dyn_mf);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);

  dynamics.set(dur_h, dyn_pp);
  left_hand.chord(dur_e, "bf3", "d4", "g4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "bf3", "d4", "g4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "c4", "ef4", "g4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);

  // p3.1
  printf("page 3.1 lh time = %f\n", left_hand.getTime());
  printf("page 3.1 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "bf3", "d4", "g4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "fs3", "c4", "ef4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  
  dynamics.rest(dur_q);
  dynamics.ramp(dur_q, dyn_mf);
  left_hand.chord(dur_e, "g3", "bf3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);
  
  dynamics.set(dur_h, dyn_mf);
  left_hand.chord(dur_e, "e3", "gs3", "e4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);

  dynamics.ramp(dur_h, dyn_mf);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);

  // p3.2
  printf("page 3.2 lh time = %f\n", left_hand.getTime());
  printf("page 3.2 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.set(dur_h, dyn_pp);
  left_hand.rest(dur_q);
  left_hand.note(dur_e, "e4", staccato);
  left_hand.note(dur_e, "c4", staccato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "a3", staccato);
  left_hand.note(dur_e, "f3", staccato);
  left_hand.note(dur_e, "a3", staccato);
  left_hand.note(dur_e, "c4", staccato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_q, "e4", legato);
  left_hand.note(dur_e, "e4", staccato);
  left_hand.note(dur_e, "c4", staccato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "a3", staccato);
  left_hand.note(dur_e, "f3", staccato);
  left_hand.note(dur_e, "a3", staccato);
  left_hand.note(dur_e, "c4", staccato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  // p3.3
  printf("page 3.3 lh time = %f\n", left_hand.getTime());
  printf("page 3.3 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "ef4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "cs4", legato);
  left_hand.note(dur_s, "d4", legato);
  left_hand.note(dur_s, "df4", legato);
  left_hand.note(dur_s, "c4", legato);
  left_hand.note(dur_s, "b3", legato);
  right_hand.note(dur_e, "e5", legato);
  right_hand.rest(dur_e + dur_q);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "b3", legato);
  right_hand.note(dur_s, "bf3", legato);
  right_hand.note(dur_s, "a3", legato);
  right_hand.note(dur_s, "af3", legato);
  right_hand.note(dur_s, "g3", legato);
  right_hand.note(dur_s, "fs3", legato);
  right_hand.note(dur_s, "f3", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  right_hand.rest(dur_q);
  right_hand.note(dur_e, "e5", staccato);
  right_hand.note(dur_e, "c5", staccato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  right_hand.note(dur_e, "a4", staccato);
  right_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_e, "a4", staccato);
  right_hand.note(dur_e, "c5", staccato);

  // p3.4
  printf("page 3.4 lh time = %f\n", left_hand.getTime());
  printf("page 3.4 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  right_hand.note(dur_q, "e5", legato);
  right_hand.note(dur_e, "e5", staccato);
  right_hand.note(dur_e, "c5", staccato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "ds3", legato);
  right_hand.note(dur_e, "a4", staccato);
  right_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_e, "a4", staccato);
  right_hand.note(dur_e, "c5", staccato);

  dynamics.rest(dur_h);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "f3", legato);
  left_hand.note(dur_s, "fs3", legato);
  left_hand.note(dur_s, "g3", legato);
  left_hand.note(dur_s, "gs3", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "as3", legato);
  left_hand.note(dur_s, "b3", legato);
  right_hand.note(dur_q, "e5", legato);
  right_hand.rest(dur_q);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c4", legato);
  right_hand.note(dur_s, "cs4", legato);
  right_hand.note(dur_s, "d4", legato);
  right_hand.note(dur_s, "ds4", legato);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);

  // p3.5
  printf("page 3.5 lh time = %f\n", left_hand.getTime());
  printf("page 3.5 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "e4", "c5", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.rest(dur_q);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "c5", staccato);
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a4", staccato);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);

  // p3.6
  printf("page 3.6 lh time = %f\n", left_hand.getTime());
  printf("page 3.6 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "e4", "c5", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.rest(dur_q);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "c5", staccato);
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a4", staccato);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "e4", "c5", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f4", "a4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "e4", "a4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d4", "e4", "gs4", "b4", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);

  // p4.1
  printf("page 4.1 lh time = %f\n", left_hand.getTime());
  printf("page 4.1 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "c4", "e4", "a4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f4", "a4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "e4", "a4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d4", "e4", "gs4", "b4", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "cs4", "g4", "a4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d4", "f4", "a4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "gs5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d4", "g4", "b4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);

  // p4.2
  printf("page 4.2 lh time = %f\n", left_hand.getTime());
  printf("page 4.2 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "gs5", legato);
  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d4", "g4", "b4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);

  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "g4", "bf4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "e4", "g4", "a5", "cs5", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "gs5", legato);

  // p4.3
  printf("page 4.3 lh time = %f\n", left_hand.getTime());
  printf("page 4.3 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "f4", "a4", "d5", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);
  
  dynamics.ramp(dur_h, dyn_pp);
  left_hand.chord(dur_e, "e3", "a3", "c4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "e3", "gs3", "b3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds6", legato);
  
  dynamics.set(dur_h, dyn_f);
  left_hand.chord(dur_e, "a2", "e3", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "e3", "c4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds6", legato);  
  
  dynamics.rest(dur_h);
  left_hand.note(dur_e, "c4", staccato);
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a3", staccato);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "ds6", legato);
  
  // p4.4
  printf("page 4.4 lh time = %f\n", left_hand.getTime());
  printf("page 4.4 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a2", "e3", "", "", "", staccato, small_arpeggio);
  left_hand.chord(dur_e, "e3", "c4", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ds6", legato);  

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "c4", "e4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "fs3", "a3", "d4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "ds6", legato);

  dynamics.ramp(dur_h, dyn_f);
  left_hand.chord(dur_e, "a3", "c4", "e4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a4", staccato);
  left_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_e, "e6", legato);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "as5", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "d4", staccato);
  left_hand.note(dur_e, "b3", staccato);
  left_hand.note(dur_e, "d4", staccato);
  left_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);

  // p4.5
  printf("page 4.5 lh time = %f\n", left_hand.getTime());
  printf("page 4.5 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.set(dur_h, dyn_mp);
  left_hand.chord(dur_e, "a3", "c4", "e4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "as5", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "ds6", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "e3", "gs3", "d4", "e4", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ds6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ds6", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "c4", "e4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a4", staccato);
  left_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_e, "e6", legato);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "as5", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);

  dynamics.rest(dur_h);
  left_hand.note(dur_e, "d4", staccato);
  left_hand.note(dur_e, "b3", staccato);
  left_hand.note(dur_e, "d4", staccato);
  left_hand.note(dur_e, "f4", staccato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "bf5", legato);
  right_hand.note(dur_s, "a5", legato);

  // p 5.1
  printf("page 5.1 lh time = %f\n", left_hand.getTime());
  printf("page 5.1 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "c4", "e4", "a4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "as5", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.ramp(dur_h, dyn_mp);
  left_hand.chord(dur_e, "e3", "gs3", "d4", "e4", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ds6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "fs6", legato);
  right_hand.note(dur_s, "g6", legato);
  right_hand.note(dur_s, "gs6", legato);

  dynamics.set(dur_h, dyn_pp);
  left_hand.chord(dur_e, "a3", "e4", "c5", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "a3", "cs4", "g4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a6", legato);
  right_hand.note(dur_s, "af6", legato);
  right_hand.note(dur_s, "g6", legato);
  right_hand.note(dur_s, "fs6", legato);
  right_hand.note(dur_s, "g6", legato);
  right_hand.note(dur_s, "fs6", legato);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "d4", "f4", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "f6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "ef6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "df6", legato);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "b5", legato);
  right_hand.note(dur_s, "bf5", legato);


  // p 5.2
  printf("page 5.2 lh time = %f\n", left_hand.getTime());
  printf("page 5.2 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "a2", "cs3", "g3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "af5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a2", "d3", "f3", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ef5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "df5", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "bf4", legato);

  dynamics.ramp(dur_q, dyn_p);
  dynamics.ramp(dur_q, dyn_f);
  left_hand.note(dur_e, "a2", staccato);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "gs3", "bf3", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "bf4", legato);
  right_hand.note(dur_s, "gs4", legato);

  dynamics.set(dur_h, dyn_p);
  left_hand.note(dur_s, "a2", legato);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "gs3", legato * 2.0);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "g3", legato * 2.0);
  left_hand.note(dur_s, "a3", legato);
  left_hand.note(dur_s, "f3", legato * 2.0);
  left_hand.note(dur_s, "a3", legato);
  right_hand.note(dur_e, "a4", staccato);
  right_hand.note(dur_e, "b4", staccato);
  right_hand.note(dur_e, "c5", staccato);
  right_hand.note(dur_e, "d5", staccato);

  // p 5.3
  printf("page 5.3 lh time = %f\n", left_hand.getTime());
  printf("page 5.3 rh time = %f\n", right_hand.getTime());
  printf("\n");

  dynamics.ramp(dur_q, dyn_p);
  dynamics.ramp(dur_q, dyn_f);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "ds5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "ds5", legato);

  dynamics.set(dur_h, dyn_p);
  left_hand.note(dur_s, "e3", legato);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato * 2.0);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "d4", legato * 2.0);
  left_hand.note(dur_s, "e4", legato);
  left_hand.note(dur_s, "b3", legato * 2.0);
  left_hand.note(dur_s, "e4", legato);
  right_hand.chord(dur_e, "e5", "", "", "", "", staccato, small_arpeggio);
  right_hand.chord(dur_e, "e5", "fs5", "", "", "", staccato, small_arpeggio);
  right_hand.chord(dur_e, "e5", "g5", "", "", "", staccato, small_arpeggio);
  right_hand.chord(dur_e, "d5", "gs5", "", "", "", staccato, small_arpeggio);


  dynamics.rest(dur_q);
  dynamics.set(dur_q, dyn_pp);
  left_hand.note(dur_q, "a3", staccato);
  left_hand.rest(dur_q);
  right_hand.chord(dur_q, "c5", "a5", "", "", "", staccato, small_arpeggio);
  right_hand.note(dur_s, "e4", legato);
  right_hand.note(dur_s, "f4", legato);
  right_hand.note(dur_s, "fs4", legato);
  right_hand.note(dur_s, "g4", legato);

  dynamics.ramp(dur_h, dyn_pp);
  left_hand.rest(dur_h);
  right_hand.note(dur_s, "gs4", legato);
  right_hand.note(dur_s, "a4", legato);
  right_hand.note(dur_s, "as4", legato);
  right_hand.note(dur_s, "b4", legato);
  right_hand.note(dur_s, "c5", legato);
  right_hand.note(dur_s, "cs5", legato);
  right_hand.note(dur_s, "d5", legato);
  right_hand.note(dur_s, "ds5", legato);

  // p 5.4
  printf("page 5.4 lh time = %f\n", left_hand.getTime());
  printf("page 5.4 rh time = %f\n", right_hand.getTime());
  printf("page 5.4 dy time = %f\n", dynamics.getTime());
  printf("\n");


  dynamics.set(dur_h, dyn_f);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e5", legato);
  right_hand.note(dur_s, "f5", legato);
  right_hand.note(dur_s, "fs5", legato);
  right_hand.note(dur_s, "g5", legato);
  right_hand.note(dur_s, "gs5", legato);
  right_hand.note(dur_s, "a5", legato);
  right_hand.note(dur_s, "as5", legato);
  right_hand.note(dur_s, "b5", legato);

  dynamics.ramp(dur_h, dyn_f);
  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "c6", legato);
  right_hand.note(dur_s, "cs6", legato);
  right_hand.note(dur_s, "d6", legato);
  right_hand.note(dur_s, "ds6", legato);
  right_hand.note(dur_s, "e6", legato);
  right_hand.note(dur_s, "fs6", legato);
  right_hand.note(dur_s, "g6", legato);
  right_hand.note(dur_s, "gs6", legato);

  dynamics.set(dur_h, dyn_pp);
  left_hand.chord(dur_q, "a2", "e3", "c4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_q);
  right_hand.note(dur_q, "a6", staccato);
  right_hand.rest(dur_q);

  dynamics.rest(dur_h);
  left_hand.chord(dur_e, "a3", "c4", "e4", "", "", staccato, rolled_chord);
  left_hand.rest(dur_e + dur_q);
  right_hand.chord(dur_e, "a4", "c5", "e5", "a6", "", staccato, rolled_chord);
  right_hand.rest(dur_e + dur_q);

  dynamics.rest(dur_q + dur_e);
  dynamics.ramp(dur_e, 0);
  left_hand.chord(dur_e, "a1", "a2", "", "", "", staccato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_e, "a3", staccato);
  right_hand.rest(dur_e + dur_q);

  // fin
  printf("page 5.5 lh time = %f\n", left_hand.getTime());
  printf("page 5.5 rh time = %f\n", right_hand.getTime());
  printf("page 5.5 dy time = %f\n", dynamics.getTime());
  printf("\n");


  // ready, set, go!
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
