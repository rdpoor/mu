/*
 * Together Now, Very Minor.  Bass Part.
 */
#include "bass.h"
#include "layout.h"

#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "null_stream.h"
#include "stream.h"

#include <map>

// ================================================================
/*
 * StreamSet maps between a pitch number (i.e. midi pitch) and
 * a stream that produces that pitch.  A typical subclass maps
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

  StreamSet *getStreamSet( void ) { return stream_set_; }
  Hand& setStreamSet(StreamSet *stream_set) { stream_set_ = stream_set; return *this; }

  mu::Stream *getStream( void ) { return &add_stream_; }

protected:
  // add a note to the stream.  Does not alter time_.
  Hand& make_note(double time, double duration, std::string pitch, double legato) {
    if (pitch.empty()) return *this;

    mu::Stream *s = stream_set_->findStream(pitch);
    mu::CropStream *cs = &(new mu::CropStream())->
      setStart(0).
      setEnd((mu::Tick)(duration * legato)).
      setSource(s);
    mu::DelayStream *ds = &(new mu::DelayStream())->
      setDelay((mu::Tick)time).
      setSource(cs);
    mu::FadeStream *fs = &(new mu::FadeStream())->
      setFadeTime(100).
      setSource(ds);
    add_stream_.addSource(fs);
    return *this;
  }

  double time_;
  mu::AddStream add_stream_;
  StreamSet *stream_set_;
};                              // class Hand

mu::Stream *make_bass_stream() {
  FileReadStreamSet *stream_set = new FileReadStreamSet();
  Hand *bass = new Hand();

  double normal_legato = 1.0;
  double dur_q = BEAT_TO_FRAME(1);
  double dur_e = dur_q / 2.0;
  double dur_h = dur_q * 2.0;
  double dur_w = dur_q * 4.0;

  stream_set->setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  bass->setStreamSet(stream_set);

  // Intro
  bass->setTime(MEASURE_TO_FRAME(M_INTRO));
  bass->rest(4 * dur_w);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs3", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs3", normal_legato);

  // Verse 1
  bass->setTime(MEASURE_TO_FRAME(M_VERS1));
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "fs1", normal_legato);
  bass->note(dur_q + dur_e, "d1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);

  // Verse 2
  bass->setTime(MEASURE_TO_FRAME(M_VERS2));
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "fs1", normal_legato);
  bass->note(dur_q + dur_e, "d1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);

  // Chorus 1
  bass->setTime(MEASURE_TO_FRAME(M_CHOR1));
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  // Verse 3
  bass->setTime(MEASURE_TO_FRAME(M_VERS3));
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "fs1", normal_legato);
  bass->note(dur_q + dur_e, "d1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "e1", normal_legato);

  // Chorus 2
  bass->setTime(MEASURE_TO_FRAME(M_CHOR2));
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "as1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e1", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  // Bridge

  bass->setTime(MEASURE_TO_FRAME(M_BRIDG));
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "gs2", normal_legato);
  bass->note(dur_h + dur_e, "gs2", normal_legato);
  bass->note(dur_q + dur_e, "gs2", normal_legato);
  bass->note(dur_h + dur_e, "gs2", normal_legato);

  bass->note(dur_q + dur_e, "b1", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "fs2", normal_legato);

  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "a2", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "e2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "cs2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(4 * dur_q, "cs2", normal_legato);

  // Outro
  bass->setTime(MEASURE_TO_FRAME(M_OUTRO));

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);
  bass->note(dur_q + dur_e, "fs2", normal_legato);
  bass->note(dur_h + dur_e, "b1", normal_legato);
  bass->note(dur_q + dur_e, "e2", normal_legato);
  bass->note(dur_h + dur_e, "cs2", normal_legato);

  // Fin.
  bass->setTime(MEASURE_TO_FRAME(M_END));

  return bass->getStream();
}
