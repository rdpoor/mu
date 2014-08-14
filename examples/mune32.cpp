/*
 * Sketch for a strummed, fretted instrument with a fixed number of
 * strings.  See also mune23 for an earlier version.
 *
 * TODO:
 * - hammer on / hammer off.  Switches from one note to another, as
 *   if the second note had the same attack time as the first.  Figure
 *   out syntax for specifying this (negative pitch number?)
 *
 * - SpliceSP: a SampleProcessor with a source, start time and end
 *   time and fade time.  It fades in at start time and fades out and
 *   end time.  Useful for splicing segments together.
 *
 * - MixSP: a general-purpose mixing bus with an arbitrary number of
 *   sources.  It mixes all inputs together.  For efficiency, it keeps
 *   sources sorted by start time and only mixes sources that are
 *   actively contributing to the output.
 */
#include "mu.h"

#include "add_sp.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "file_read_sp.h"
#include "multi_source_sp.h"
#include "multiply_sp.h"
#include "null_sp.h"
#include "rt_player.h"
#include "single_source_sp.h"
#include "tick_utils.h"

#include <map>

// ================================================================
// class SpliceSP : public mu::SingleSourceSP<SpliceSP> {
  // for now, delgate to mu::CropSP
// };

// ================================================================
// class MixSP : public mu::MultiSourceSP<MixSP> {
  // for now, delegate to mu::AddSP
// };

// ================================================================
// SPSet maps an integer pitch number to a SampleProcessor that
// produces that pitch.  Its subclass (FileReadSPSet) maps from a
// pitch number to a sound file.

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
class FrettedString {
public:
  static const int kDamped = -1;
  static const int kSustain = -2;

  FrettedString( int open_pitch,  mu::AddSP *output, FileReadSPSet *sp_set ) {
    open_pitch_ = open_pitch;
    output_ = output;
    sp_set_ = sp_set;
    prev_sp_ = NULL;
    prev_start_ = 0;
  }

  FrettedString& pluck(mu::Tick start, int fret) {
    if (fret == kSustain) {
      return *this;
    } else if (fret == kDamped) {
      return emitSegment(start);
    } else {
      return pluckAux(start, findSP(fret));
    }
  }

protected:
  // This starts a new segment.  It won't be emitted until we know the
  // end time, e.g. though another call to pluckAux() or an explicit
  // call to emitSegment().
  FrettedString &pluckAux(mu::Tick start, mu::SampleProcessor *sp) {
    emitSegment(start);
    prev_sp_ = sp;
    prev_start_ = start;
    return *this;
  }

  FrettedString &emitSegment(mu::Tick start) {
    mu::SampleProcessor *segment = createSegment(start);
    if (segment != NULL) {
      output_->addSource(segment);
      prev_sp_ = NULL;
    }
    return *this;
  }
  
  mu::SampleProcessor *createSegment(mu::Tick start) {
    if (prev_sp_ == NULL) return NULL;
    // SpliceSP *spliceSP = new SpliceSP();
    mu::CropSP *spliceSP = new mu::CropSP();
    spliceSP->setStart(0);
    spliceSP->setEnd(start-prev_start_);
    spliceSP->setSource(prev_sp_);
    mu::DelaySP *delaySP = &(new mu::DelaySP())->
      setSource(spliceSP).
      setDelay(prev_start_);
    return delaySP;
  }

  mu::SampleProcessor *findSP(int fret) {
    mu::SampleProcessor *sp = sp_set_->findSP(open_pitch_ + fret);
    return (sp == NULL) ? &null_sp_ : sp;
  }

  int open_pitch_;
  FileReadSPSet *sp_set_;
  mu::AddSP *output_;
  mu::NullSP null_sp_;
  mu::SampleProcessor *prev_sp_;
  mu::Tick prev_start_;
};

// ================================================================
class FrettedInstrument {

public:

  FrettedInstrument( void ) {
  }
  ~FrettedInstrument( void ) {
  }

  FileReadSPSet *getSPSet() { return stream_set_; }
  FrettedInstrument& setSPSet( FileReadSPSet *stream_set ) { stream_set_ = stream_set; return *this; }

  mu::AddSP *getOutput() {
    return &output_;
  }

  FrettedInstrument& addString(int open_pitch) {
    strings_.push_back(new FrettedString(open_pitch, getOutput(), getSPSet()));
    return *this;
  }

  /*
   * Generate a strum.  If rate is positive, it's a downstroke.  If
   * negative, it's an upstroke.
   *
   * Frets specifies the fret number (including 0 for open), and
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
    for (long int i=0; i<strings_.size();  i++) {
      FrettedString *s = strings_.at(i);
      s->pluck(start + (i * rate), frets[i]);
    }
    return *this;
  }

  FrettedInstrument& damp(mu::Tick end) {
    for (long int i=0; i<strings_.size(); i++) {
      FrettedString *s = strings_.at(i);
      s->pluck(end, FrettedString::kDamped);
    }
    return *this;
  }

protected:

  std::vector<FrettedString *> strings_;
  mu::AddSP output_;
  FileReadSPSet *stream_set_;
};

// ================================================================
int main() {
  mu::RtPlayer player;
  FileReadSPSet file_read_sp_set;
  FrettedInstrument fretted_instrument;

  file_read_sp_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  fretted_instrument.setSPSet(&file_read_sp_set);

  // six string open D tuning
  fretted_instrument.addString(38+12); // D3
  fretted_instrument.addString(45+12); // A3
  fretted_instrument.addString(50+12); // D4
  fretted_instrument.addString(54+12); // F#4
  fretted_instrument.addString(57+12); // A4
  fretted_instrument.addString(62+12); // D5
  
#define T_STROKE (44100 * 0.01)
#define TQ (44100 * 0.6)
#define TE (TQ/2)
#define TS (TQ/4)
  mu::Tick t = 0;

  fretted_instrument.strum(t, (int []){0, 0, 0, 0, 0, 0}, T_STROKE); // downstroke
  t += TS;
  fretted_instrument.strum(t, (int []){0, 0, 0, 0, 0, 0}, -T_STROKE); // upstroke
  t += TS;
  fretted_instrument.strum(t, (int []){0, 0, 0, 0, 0, 0}, T_STROKE); // downstroke
  t += TQ;
  fretted_instrument.strum(t, (int []){0, 2, 0, 1, 0, 0}, T_STROKE); // downstroke
  t += TQ;
  fretted_instrument.strum(t, (int []){0, 0, 0, 0, 0, 0}, 3 * T_STROKE); // slow downstroke
  t += 4 * TQ;
  fretted_instrument.damp(t); // final stop

  mu::MultiplySP multiply_sp;
  mu::ConstantSP constant_sp;

  // add gain control
  constant_sp.setValue(0.25);
  multiply_sp.addSource(fretted_instrument.getOutput());
  multiply_sp.addSource(&constant_sp);

  player.setSource(&multiply_sp);
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  std::cout << fretted_instrument.getOutput()->inspect();


}
