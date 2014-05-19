/*
 * Test FadeStream with sound samples.
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

mu::Stream *makeNote(StreamSet *ss, mu::Tick start, mu::Tick duration, int pitch) {
  mu::Stream *s = ss->findStream(pitch);
#if 0
  mu::CropStream *cs = &(new mu::CropStream())->
    setStart(0).
    setEnd(duration * 1.5).
    setSource(s);
  mu::DelayStream *ds = &(new mu::DelayStream())->
    setDelay(start).
    setSource(cs);
  mu::FadeStream *fs = &(new mu::FadeStream())->
    setFadeTime(200).
    setSource(ds);
  return fs;
#else
  mu::CropStream *cs = &(new mu::CropStream())->
    setStart(0).
    setEnd(duration * 1.5).
    setSource(s);
  mu::DelayStream *ds = &(new mu::DelayStream())->
    setDelay(start).
    setSource(cs);
  return ds;
#endif
}

// ================================================================
int main() {
  mu::RtPlayer player;
  mu::AddStream as;
  FileReadStreamSet ss;

  ss.setDirectoryName("/Users/r/Projects/Mu/SoundSets/A/");

#define TQ (44100 * 0.4)
#define TE (TQ/2)
#define TS (TQ/4)
#define TT (TQ/8)

  mu::Tick t;
  
  t = 0;
  for (int i=60; i<=83; i++) {
    bool is_last = i == 83;
    as.addSource(makeNote(&ss, t, is_last ? 44100 : TT, i));
    t += TT;
  }

  player.setSource(&as);
  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
