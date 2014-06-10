/*
 * Build a Strummed instrument using FadeStream and customized
 * (sketch) AddStream.
 *
 * See also mune12.cpp, mune17.cpp, mune22.cpp, mune23.cpp
 *
 * StrummedInstrument defines an N-stringed, fretted instrument.  Any
 * one string can be plucked, damped or allowed to ring at the same
 * time as other strings, but a single string will never play two
 * notes at once.
 *
 * Despite using a FadeStream, this still creates lots of crackles
 * at the transition from one note to the next.  See mune25 for a
 * simpler experiement...
 */
#include "mu.h"

#include "add_stream.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_read_stream.h"
#include "null_stream.h"
#include "multiply_stream.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

// ================================================================
/*
 * SeqStream accepts a number of stream and sorts them by start time.
 * When source stream N+1 is scheduled to start, stream N fades out and
 * stream N+1 fades in.  This is a short crossfade to avoid clicks.
 */

class SeqStream : public mu::AddStream {
public:
  static const int kDefaultFadeTime = 20;

  SeqStream &addSource(Stream *source) {
    AddStream::addSource(&(new mu::FadeStream())->
                         setSource(source).setFadeTime(kDefaultFadeTime));
    sortSources();
    setFadeStartAndEndTimes();
    return *this;
  }

  void sortSources();
  
  static bool sortPredicate(mu::Stream *s0, mu::Stream *s1);

  void setFadeStartAndEndTimes() {
    mu::FadeStream *prev = NULL;
    for (long int i=0; i<sources_.size(); i++) {
      // This cast is guaranteed to be legit because we added the
      // fade stream in addSource() (above).
      mu::FadeStream *curr = (mu::FadeStream *)sources_.at(i);
      curr->setEnd(mu::kIndefinite);
      if (prev == NULL) {
        curr->setStart(mu::kIndefinite);
      } else {
        // Set crossfade to the start time of this source.
        mu::Tick t = curr->getSource()->getStart();
        prev->setEnd(t);
        curr->setStart(t);
      }
      prev = curr;
    }
  }

};                              // class SeqStream

void SeqStream::sortSources() {
  std::sort(sources_.begin(), sources_.end(), sortPredicate);
}

// A sorting predicate that sorts elements according to the start
// time of each source.  A start time of kIndefinite sorts earlier
// than all other times.
bool SeqStream::sortPredicate(mu::Stream *s0, mu::Stream *s1) {
  if (s0->getStart() == mu::kIndefinite) {
    return true;
  } else if (s1->getStart() == mu::kIndefinite) {
    return false;
  } else {
    return (s0->getStart() < s1->getStart());
  }
}

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
  virtual mu::Stream *findStream(int pitch) = 0;

protected:
  mu::NullStream null_stream_;
};                              // class StreamSet

// ================================================================
class FileReadStreamSet : public StreamSet {
public:
  mu::Stream *findStream(int pitch) {
    if (cache_.find(pitch) == cache_.end()) {
      std::string file_name = makeFileName(pitch);
      mu::FileReadStream *frs = new mu::FileReadStream();
      frs->fileName(file_name).doNormalize(true);
      cache_[pitch] = frs;
    }
    return cache_[pitch];
  }

  std::string getDirectoryName( void ) { return directory_name_; }
  FileReadStreamSet& setDirectoryName(std::string directory_name) { 
    directory_name_ = directory_name; 
    return *this;
  }

protected:
  std::string makeFileName(int pitch) {
    std::stringstream ss;
    ss << directory_name_ << pitch << ".wav";
    return ss.str();
  }
  std::string directory_name_;
  std::map<int, mu::FileReadStream *> cache_;
};                              // class FileReadStreamSet


// ================================================================
class FrettedInstrument {
public:
  static const int kDamped = -1;
  static const int kSustain = -2;

  FrettedInstrument( void ) {
  }
  ~FrettedInstrument( void ) {
  }

  StreamSet *getStreamSet() { return stream_set_; }
  FrettedInstrument& setStreamSet( StreamSet *stream_set ) { stream_set_ = stream_set; return *this; }

  FrettedInstrument& addString(int open_tuning) {
    open_pitches_.push_back(open_tuning);
    SeqStream *seq_stream = new SeqStream();
    strings_.push_back(seq_stream);
    output_.addSource(seq_stream);
    return *this;
  }
  /*
   * Generate a strum by adding to each SeqStream.  Strum is offset
   * to start time.  If rate is positive, it's a downstroke.  If
   * negative, it's an upstroke.
   *
   * Frets speficied the fret number (including 0 for open), and
   * recognizes the following special values:
   *
   *  FrettedInstrument::kDamped -- string does not play
   *  FrettedInstrument::kSustain -- prev note allowed to ring.
   *
   * nb: \c frets must contain exactly one item per string.
   */
  FrettedInstrument& strum(mu::Tick start, int frets[], mu::Tick rate) {
    if (rate < 0) {
      start = start + (1-strings_.size()) * rate;
    }
    for (long int i=strings_.size()-1; i>=0; i--) {
      strum_aux(start + (i*rate), strings_.at(i), open_pitches_[i], frets[i]);
    }
    return *this;
  }

  mu::Stream *getStream() { return &output_; }

protected:
  
  void strum_aux(mu::Tick start, mu::Stream *stream, int open_pitch, int fret) {
    SeqStream *seq_stream = (SeqStream *)stream;
    if (fret == kSustain) {
      // sustain just means to let previous stream run.
    } else if (fret == kDamped) {
      // null_stream has indefinite extent.  CropStream forces it to
      // start at the desired time.
      mu::CropStream *cs = &(new mu::CropStream())->
        setStart(start).
        setSource(&null_stream_);
      seq_stream->addSource(cs);
    } else {
      mu::Stream *s = stream_set_->findStream(open_pitch + fret);
      if (s != NULL) {
        mu::DelayStream *ds = &(new mu::DelayStream())->
          setDelay(start).
          setSource(s);
        seq_stream->addSource(ds);
      }
    }
  }

  std::vector<int> open_pitches_;
  mu::StreamVector strings_;
  mu::AddStream output_;
  mu::NullStream null_stream_;
  StreamSet *stream_set_;

};                              // class FrettedInstrument

// ================================================================
int main() {
  mu::RtPlayer player;
  FileReadStreamSet file_read_stream_set;
  FrettedInstrument fretted_instrument;

  file_read_stream_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  fretted_instrument.setStreamSet(&file_read_stream_set);

  fretted_instrument.addString(67); // 392.0 hz, G4
  fretted_instrument.addString(60); // 261.6 hz, C4
  fretted_instrument.addString(64); // 329.6 Hz, E4
  fretted_instrument.addString(69); // 440.0 Hz, A4

  // fretted_instrument.strum(0, (int []){0, 0, 0, 0}, 2000);
  // fretted_instrument.strum(11025, (int []){1, 1, 1, 1}, -2000);

#define TQ (44100 * 0.4)
#define TE (TQ/2)
#define TS (TQ/4)
#define TDE (TE*1.6)

  mu::Tick t;
  t = 0;
  fretted_instrument.strum(t, (int []){3, 2, 1, 1}, 800); t += TDE;
  fretted_instrument.strum(t, (int []){3, 2, 1, 1}, -800); t += (TQ - TDE);
  fretted_instrument.strum(t, (int []){3, 2, 1, 1}, 800); t += TE;
  fretted_instrument.strum(t, (int []){-1, -1, -1, -1}, -800); t += (TQ - TE);

  fretted_instrument.strum(t, (int []){1, 2, 1, 2}, 800); t += TDE;
  fretted_instrument.strum(t, (int []){1, 2, 1, 2}, -800); t += (TQ - TDE);
  fretted_instrument.strum(t, (int []){1, 2, 1, 2}, 800); t += TE;
  fretted_instrument.strum(t, (int []){-1, -1, -1, -1}, -800); t += (TQ - TE);

  fretted_instrument.strum(t, (int []){3, 3, 3, 3}, 800); t += TDE;
  fretted_instrument.strum(t, (int []){3, 3, 3, 3}, -800); t += (TQ - TDE);
  fretted_instrument.strum(t, (int []){3, 3, 3, 3}, 800); t += TE;
  fretted_instrument.strum(t, (int []){-1, -1, -1, -1}, -800); t += (TQ - TE);

  fretted_instrument.strum(t, (int []){2, 3, 1, 0}, 4000); t += TDE;

  mu::MultiplyStream multiply_stream;
  mu::ConstantStream constant_stream;

  // gain control?
  constant_stream.setValue(0.25);
  multiply_stream.addSource(fretted_instrument.getStream());
  multiply_stream.addSource(&constant_stream);

  player.setSource(&multiply_stream);
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  std::cout << multiply_stream.inspect();

  return 0;
}
