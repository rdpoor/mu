/*
 * Sketch for Flight of the Bumble Bee.
 *
 * I need to decide if (and how) to score this.  Fow now, I'll
 * just use a compact C++ notation to test a few bars.
 */
#include "mu.h"

#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "null_stream.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

// ================================================================
/*
 * StreamSet maps between a pitch number (i.e. midi pitch) and
 * a stream that produces that pitch.  A normal example is mapping
 * a pitch to a FileReadStream().
 */
class StreamSet {
public:
  StreamSet( void ) {
  }
  ~StreamSet( void ) {
  }
  virtual mu::Stream *findStream(std::string name) = 0;

protected:
  mu::NullStream null_stream_;
};                              // class StreamSet

// ================================================================
class FileReadStreamSet : public StreamSet {
public:
  mu::Stream *findStream(std::string name) {
    if (cache_.find(name) == cache_.end()) {
      std::string file_name = makeFileName(name);
      mu::FileReadStream *frs = new mu::FileReadStream();
      frs->fileName(file_name).doNormalize(true);
      cache_[name] = frs;
    }
    return cache_[name];
  }

  std::string getDirectoryName( void ) { return directory_name_; }
  FileReadStreamSet& setDirectoryName(std::string directory_name) { 
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
  std::map<std::string, mu::FileReadStream *> cache_;
};                              // class FileReadStreamSet

// ================================================================

/*
 * Generate notes, chords and rests
 */
class Hand {
public:
  // generate a rest by incrementing time by duration.
  Hand& rest(mu::Tick duration) {
    time_ += duration;
    return *this;
  }

  // generate a note and increment time by duration.
  Hand& note(mu::Tick duration, std::string pitch, double legato=1.5) {
    make_note(time_, duration, pitch, legato);
    time_ += duration;
    return *this;
  }

  // play up to 5 notes as a chord.  arpeggio rolls the chord as a
  // factor of the duration: 0 = no roll, 1.0 = roll over duration.
  Hand& chord(mu::Tick duration, 
              std::string n0 = "",
              std::string n1 = "",
              std::string n2 = "",
              std::string n3 = "",
              std::string n4 = "",
              double legato = 1.0,
              double arpeggio = 0.0) {
    if (!n0.empty()) make_note(time_ + 0.0 * arpeggio * duration, duration, n0, legato);
    if (!n1.empty()) make_note(time_ + 1.0 * arpeggio * duration, duration, n1, legato);
    if (!n2.empty()) make_note(time_ + 2.0 * arpeggio * duration, duration, n2, legato);
    if (!n3.empty()) make_note(time_ + 3.0 * arpeggio * duration, duration, n3, legato);
    if (!n4.empty()) make_note(time_ + 4.0 * arpeggio * duration, duration, n4, legato);
    time_ += duration;
    return *this;
  }

  mu::Tick getTime( void ) { return time_; }
  Hand& setTime(mu::Tick time) { time_ = time; return *this; }

  StreamSet *getStreamSet( void ) { return stream_set_; }
  Hand& setStreamSet(StreamSet *stream_set) { stream_set_ = stream_set; return *this; }

  mu::Stream *getStream( void ) { return &add_stream_; }

protected:
  // add a note to the stream.  Does not alter time_.
  Hand& make_note(mu::Tick time, mu::Tick duration, std::string pitch, double legato) {
    mu::Stream *s = stream_set_->findStream(pitch);
    mu::CropStream *cs = &(new mu::CropStream())->
      setStart(0).
      setEnd(duration * legato).
      setSource(s);
    mu::DelayStream *ds = &(new mu::DelayStream())->
      setDelay(time).
      setSource(cs);
    mu::FadeStream *fs = &(new mu::FadeStream())->
      setFadeTime(100).
      setSource(ds);
    add_stream_.addSource(fs);
    return *this;
  }

  mu::Tick time_;
  mu::AddStream add_stream_;
  StreamSet *stream_set_;
};                              // class Hand

// ================================================================

int main() {
  FileReadStreamSet stream_set;
  Hand right_hand;
  Hand left_hand;
  mu::AddStream add_stream;
  mu::RtPlayer player;

  stream_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  right_hand.setStreamSet(&stream_set);
  left_hand.setStreamSet(&stream_set);
  
  // mix left and right hands into one stream
  add_stream.addSource(right_hand.getStream());
  add_stream.addSource(left_hand.getStream());

  // play the mix stream.
  player.setSource(&add_stream);

  // create some handy durations
  mu::Tick dur_q = (44100 * 0.35); // quarter
  mu::Tick dur_e = dur_q / 2.0;    // eigth
  mu::Tick dur_s = dur_q / 4.0;    // sixteenth
  mu::Tick dur_h = dur_q * 2.0;    // half (1 measure)
  mu::Tick dur_w = dur_q * 4.0;    // whole

  // here is the score...
  right_hand.setTime(0);
  left_hand.setTime(0);

  // rest(mu::Tick duration)
  // note(mu::Tick duration, std::string pitch, double legato=1.0)
  // chord(mu::Tick duration,  n0, n1, n2, n3, n4, legato=1.0, arpeggio=0.0)

  double lh_legato=0.8;
  double lh_arpeggio=0.2;
  double small_arpeggio=0.05;
  double rh_legato=1.0;

  // p1.1
  left_hand.chord(dur_e, "e3", "b3", "gs4", "", "", lh_legato, lh_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e6", rh_legato);
  right_hand.note(dur_s, "ef6", rh_legato);
  right_hand.note(dur_s, "d6", rh_legato);
  right_hand.note(dur_s, "cs6", rh_legato);
  right_hand.note(dur_s, "d6", rh_legato);
  right_hand.note(dur_s, "cs6", rh_legato);
  right_hand.note(dur_s, "c6", rh_legato);
  right_hand.note(dur_s, "b5", rh_legato);

  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c6", rh_legato);
  right_hand.note(dur_s, "b5", rh_legato);
  right_hand.note(dur_s, "bf5", rh_legato);
  right_hand.note(dur_s, "a5", rh_legato);
  right_hand.note(dur_s, "af5", rh_legato);
  right_hand.note(dur_s, "g5", rh_legato);
  right_hand.note(dur_s, "fs5", rh_legato);
  right_hand.note(dur_s, "f5", rh_legato);

  left_hand.chord(dur_e, "e2", "b2", "gs3", "", "", lh_legato, lh_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e5", rh_legato);
  right_hand.note(dur_s, "ef5", rh_legato);
  right_hand.note(dur_s, "d5", rh_legato);
  right_hand.note(dur_s, "cs5", rh_legato);
  right_hand.note(dur_s, "d5", rh_legato);
  right_hand.note(dur_s, "cs5", rh_legato);
  right_hand.note(dur_s, "c5", rh_legato);
  right_hand.note(dur_s, "b4", rh_legato);

  left_hand.rest(dur_h);
  right_hand.note(dur_s, "c5", rh_legato);
  right_hand.note(dur_s, "b4", rh_legato);
  right_hand.note(dur_s, "bf4", rh_legato);
  right_hand.note(dur_s, "a4", rh_legato);
  right_hand.note(dur_s, "af4", rh_legato);
  right_hand.note(dur_s, "g4", rh_legato);
  right_hand.note(dur_s, "fs4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);

  // p1.2

  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "b3", rh_legato);

  left_hand.rest(dur_h);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "b3", rh_legato);

  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", lh_legato, lh_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);

  left_hand.note(dur_e, "c4");
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a3");
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);

  // p1.3

  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", lh_legato, lh_arpeggio);
  left_hand.rest(dur_e + dur_q);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);

  left_hand.note(dur_e, "c4");
  left_hand.rest(dur_e);
  left_hand.note(dur_e, "a3");
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);

  left_hand.chord(dur_e, "a2", "e3", "c4", "", "", lh_legato, lh_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "b3", rh_legato);

  left_hand.chord(dur_e, "e3", "a3", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "e3", "gs3", "b3", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);

  // p1.4

  left_hand.chord(dur_e, "c3", "e3", "a3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "f3", "a3", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "ef4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "b3", rh_legato);

  left_hand.chord(dur_e, "e3", "a3", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "e3", "gs3", "b3", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "c4", rh_legato);
  right_hand.note(dur_s, "cs4", rh_legato);
  right_hand.note(dur_s, "d4", rh_legato);
  right_hand.note(dur_s, "ds4", rh_legato);
  right_hand.note(dur_s, "e4", rh_legato);
  right_hand.note(dur_s, "fs4", rh_legato);
  right_hand.note(dur_s, "g4", rh_legato);
  right_hand.note(dur_s, "gs4", rh_legato);

  left_hand.chord(dur_e, "cs3", "g3", "a3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "f3", "a3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", rh_legato);
  right_hand.note(dur_s, "af4", rh_legato);
  right_hand.note(dur_s, "g4", rh_legato);
  right_hand.note(dur_s, "fs4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);
  right_hand.note(dur_s, "bf4", rh_legato);
  right_hand.note(dur_s, "a4", rh_legato);
  right_hand.note(dur_s, "gs4", rh_legato);

  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "b3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  right_hand.note(dur_s, "a4", rh_legato);
  right_hand.note(dur_s, "af4", rh_legato);
  right_hand.note(dur_s, "g4", rh_legato);
  right_hand.note(dur_s, "fs4", rh_legato);
  right_hand.note(dur_s, "f4", rh_legato);
  right_hand.note(dur_s, "fs4", rh_legato);
  right_hand.note(dur_s, "g4", rh_legato);
  right_hand.note(dur_s, "gs4", rh_legato);

  // p2.1

  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);
  
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "b3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "g3", "bf3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "e3", "g3", "a3", "cs4", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  
  // p2.2
  left_hand.chord(dur_e, "f3", "a3", "d4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "d3", "g3", "bf3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  
  left_hand.chord(dur_e, "d3", "f3", "a3", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "cs3", "e3", "g3", "a3", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  
  left_hand.chord(dur_s, "d3", "f3", "", "", "", 2.0, small_arpeggio); // 2x legato
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);

  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  
  // p2.3

  left_hand.rest(dur_s);
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);
  left_hand.note(dur_s, "b3", rh_legato);
  left_hand.note(dur_s, "a3", rh_legato);
  
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  left_hand.note(dur_s, "bf3", rh_legato);
  left_hand.note(dur_s, "af3", rh_legato);
  
  left_hand.note(dur_e, "bf3");
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "af3", "bf3", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);

  left_hand.chord(dur_e, "af3", "", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "af3", "bf3", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "g3", "b3", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "fs3", "c4", "", "", "", lh_legato, small_arpeggio);
  

  // p2.4

  left_hand.chord(dur_h * 2, "f3", "cs4", "", "", "", lh_legato, small_arpeggio);

  left_hand.note(dur_s, "fs3", 2.0); // 2x legato
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);

  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);

  
  // p2.5

  left_hand.rest(dur_s);
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);
  left_hand.note(dur_s, "e4", rh_legato);
  left_hand.note(dur_s, "d4", rh_legato);

  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);
  left_hand.note(dur_s, "ef4", rh_legato);
  left_hand.note(dur_s, "df4", rh_legato);

  left_hand.note(dur_e, "d4");
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "df4", "ef4", "", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);

  left_hand.chord(dur_e, "d4", "", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "df4", "ef4", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "c4", "e4", "", "", "", lh_legato, small_arpeggio);
  left_hand.chord(dur_e, "b3", "f4", "", "", "", lh_legato, small_arpeggio);
  
  // p2.6

  left_hand.chord(dur_h * 2, "bf3", "fs4", "", "", "", lh_legato, small_arpeggio);

  left_hand.chord(dur_e, "bf3", "d4", "g4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e + dur_q);

  left_hand.chord(dur_e, "bf3", "d4", "g4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);
  left_hand.chord(dur_e, "c4", "ef4", "g4", "", "", lh_legato, small_arpeggio);
  left_hand.rest(dur_e);

  // p3.1


  // ready, set, go!
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
