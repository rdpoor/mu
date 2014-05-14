/*
 * Build a Strummed instrument.
 *
 * See also mune12.cpp, mune17.cpp, mune22.cpp
 *
 * StrummedInstrument defines an N-stringed, fretted instrument.  Any
 * one string can be plucked, damped or allowed to ring at the same
 * time as other strings, but a single string will never play two
 * notes at once.
 */
#include "mu.h"

#include "file_read_stream.h"
#include "loop_stream.h"
#include "null_stream.h"
#include "rt_player.h"
#include "splice_stream.h"
#include <map>
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

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
    mu::SpliceStream *splice_stream = new mu::SpliceStream();
    strings_.push_back(splice_stream);
    output_.addSource(splice_stream);
    return *this;
  }
  /*
   * Generate a strum by adding to each SpliceStream.  Strum is offset
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
    mu::SpliceStream *splice_stream = (mu::SpliceStream *)stream;
    if (fret == kSustain) {
    } else if (fret == kDamped) {
      splice_stream->addSource(&null_stream_, start);
    } else {
      mu::Stream *s = stream_set_->findStream(open_pitch + fret);
      if (s != NULL) splice_stream->addSource(s, start);
    }
  }

  std::vector<int> open_pitches_;
  mu::StreamVector strings_;
  mu::AddStream output_;
  mu::NullStream null_stream_;
  StreamSet *stream_set_;

};                              // class FrettedInstrument


int main() {
  mu::LoopStream loop_stream;
  mu::RtPlayer player;
  FileReadStreamSet file_read_stream_set;
  FrettedInstrument fretted_instrument;

  file_read_stream_set.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");
  fretted_instrument.setStreamSet(&file_read_stream_set);

  fretted_instrument.addString(67); // 392.0 hz, G4
  fretted_instrument.addString(60); // 261.6 hz, C4
  fretted_instrument.addString(64); // 329.6 Hz, E4
  fretted_instrument.addString(69); // 440.0 Hz, A4

  // upstroke / downstroke works.  Playing two strums in a row does not.
  // fretted_instrument.strum(0, (int []){0, 0, 0, 0}, 2000);
  fretted_instrument.strum(0, (int []){0, 0, 0, 0}, 2000);
  fretted_instrument.strum(11024, (int []){-1, -1, -1, -1}, 2000);
  fretted_instrument.strum(11025, (int []){1, 1, 1, 1}, -2000);

  loop_stream.setSource(fretted_instrument.getStream());
  loop_stream.setLoopDuration(loop_stream.getSource()->getEnd());

  std::cout << loop_stream.inspect();

  player.setSource(&loop_stream);
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  std::cout << loop_stream.inspect();

  return 0;
}
