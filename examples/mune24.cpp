/*
 * Build a Strummed instrument using FadeSP and customized
 * (sketch) AddSP.
 *
 * See also mune12.cpp, mune17.cpp, mune22.cpp, mune23.cpp
 *
 * StrummedInstrument defines an N-stringed, fretted instrument.  Any
 * one string can be plucked, damped or allowed to ring at the same
 * time as other strings, but a single string will never play two
 * notes at once.
 *
 * Despite using a FadeSP, this still creates lots of crackles
 * at the transition from one note to the next.  See mune25 for a
 * simpler experiement...
 */
#include "mu.h"

#include "add_sp.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "fade_sp.h"
#include "file_read_sp.h"
#include "null_sp.h"
#include "multiply_sp.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

// ================================================================
/*
 * SeqSP accepts a number of stream and sorts them by start time.
 * When source stream N+1 is scheduled to start, stream N fades out and
 * stream N+1 fades in.  This is a short crossfade to avoid clicks.
 */

class SeqSP : public mu::AddSP {
public:
  static const int kDefaultFadeTime = 20;

  SeqSP &addSource(SampleProcessor *source) {
    AddSP::addSource(&(new mu::FadeSP())->
                         setSource(source).setFadeTime(kDefaultFadeTime));
    sortSources();
    setFadeStartAndEndTimes();
    return *this;
  }

  void sortSources();
  
  static bool sortPredicate(mu::SampleProcessor *s0, mu::SampleProcessor *s1);

  void setFadeStartAndEndTimes() {
    mu::FadeSP *prev = NULL;
    for (long int i=0; i<sources_.size(); i++) {
      // This cast is guaranteed to be legit because we added the
      // fade stream in addSource() (above).
      mu::FadeSP *curr = (mu::FadeSP *)sources_.at(i);
      curr->setEnd(mu::TickUtils::indefinite());
      if (prev == NULL) {
        curr->setStart(mu::TickUtils::indefinite());
      } else {
        // Set crossfade to the start time of this source.
        mu::Tick t = curr->getSource()->getStart();
        prev->setEnd(t);
        curr->setStart(t);
      }
      prev = curr;
    }
  }

};                              // class SeqSP

void SeqSP::sortSources() {
  std::sort(sources_.begin(), sources_.end(), sortPredicate);
}

// A sorting predicate that sorts elements according to the start
// time of each source.  A start time of TickUtils::indefinite() sorts earlier
// than all other times.
bool SeqSP::sortPredicate(mu::SampleProcessor *s0, mu::SampleProcessor *s1) {
  if (s0->getStart() == mu::TickUtils::indefinite()) {
    return true;
  } else if (s1->getStart() == mu::TickUtils::indefinite()) {
    return false;
  } else {
    return (s0->getStart() < s1->getStart());
  }
}

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
  virtual mu::SampleProcessor *findSP(int pitch) = 0;

protected:
  mu::NullSP null_sp_;
};                              // class SPSet

// ================================================================
class FileReadSPSet : public SPSet {
public:
  mu::SampleProcessor *findSP(int pitch) {
    if (cache_.find(pitch) == cache_.end()) {
      std::string file_name = makeFileName(pitch);
      mu::FileReadSP *frs = new mu::FileReadSP();
      frs->fileName(file_name).doNormalize(true);
      cache_[pitch] = frs;
    }
    return cache_[pitch];
  }

  std::string getDirectoryName( void ) { return directory_name_; }
  FileReadSPSet& setDirectoryName(std::string directory_name) { 
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
  std::map<int, mu::FileReadSP *> cache_;
};                              // class FileReadSPSet


// ================================================================
class FrettedInstrument {
public:
  static const int kDamped = -1;
  static const int kSustain = -2;

  FrettedInstrument( void ) {
  }
  ~FrettedInstrument( void ) {
  }

  SPSet *getSPSet() { return stream_set_; }
  FrettedInstrument& setSPSet( SPSet *stream_set ) { stream_set_ = stream_set; return *this; }

  FrettedInstrument& addString(int open_tuning) {
    open_pitches_.push_back(open_tuning);
    SeqSP *seq_sp = new SeqSP();
    strings_.push_back(seq_sp);
    output_.addSource(seq_sp);
    return *this;
  }
  /*
   * Generate a strum by adding to each SeqSP.  Strum is offset
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

  mu::SampleProcessor *getSP() { return &output_; }

protected:
  
  void strum_aux(mu::Tick start, mu::SampleProcessor *stream, int open_pitch, int fret) {
    SeqSP *seq_sp = (SeqSP *)stream;
    if (fret == kSustain) {
      // sustain just means to let previous stream run.
    } else if (fret == kDamped) {
      // null_sp has indefinite extent.  CropSP forces it to
      // start at the desired time.
      mu::CropSP *cs = &(new mu::CropSP())->
        setStart(start).
        setSource(&null_sp_);
      seq_sp->addSource(cs);
    } else {
      mu::SampleProcessor *s = stream_set_->findSP(open_pitch + fret);
      if (s != NULL) {
        mu::DelaySP *ds = &(new mu::DelaySP())->
          setDelay(start).
          setSource(s);
        seq_sp->addSource(ds);
      }
    }
  }

  typedef std::vector<mu::SampleProcessor *> SPVector;

  std::vector<int> open_pitches_;
  mu::SPVector strings_;
  mu::AddSP output_;
  mu::NullSP null_sp_;
  SPSet *stream_set_;

};                              // class FrettedInstrument

// ================================================================
int main() {
  mu::RtPlayer player;
  FileReadSPSet file_read_sp_set;
  FrettedInstrument fretted_instrument;

  file_read_sp_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  fretted_instrument.setSPSet(&file_read_sp_set);

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

  mu::MultiplySP multiply_sp;
  mu::ConstantSP constant_sp;

  // gain control?
  constant_sp.setValue(0.25);
  multiply_sp.addSource(fretted_instrument.getSP());
  multiply_sp.addSource(&constant_sp);

  player.setSource(&multiply_sp);
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  std::cout << multiply_sp.inspect();

  return 0;
}
