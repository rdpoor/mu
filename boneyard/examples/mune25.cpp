/*
 * Test FadeSP with sound samples.
 */
#include "mu.h"

#include "add_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "fade_sp.h"
#include "file_read_sp.h"
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

mu::SampleProcessor *makeNote(SPSet *ss, mu::Tick start, mu::Tick duration, int pitch) {
  mu::SampleProcessor *s = ss->findSP(pitch);
#if 1
  mu::CropSP *cs = &(new mu::CropSP())->
    setStart(0).
    setEnd(duration * 1.5).
    setSource(s);
  mu::DelaySP *ds = &(new mu::DelaySP())->
    setDelay(start).
    setSource(cs);
  mu::FadeSP *fs = &(new mu::FadeSP())->
    setFadeTime(100).
    setSource(ds);
  return fs;
#else
  mu::CropSP *cs = &(new mu::CropSP())->
    setStart(0).
    setEnd(duration * 3.5).
    setSource(s);
  mu::DelaySP *ds = &(new mu::DelaySP())->
    setDelay(start).
    setSource(cs);
  mu::FadeSP *fs = &(new mu::FadeSP())->
    setFadeTime(8000).
    setSource(ds);
  return fs;
#endif
}

// ================================================================
int main() {
  mu::RtPlayer player;
  mu::AddSP as;
  FileReadSPSet ss;

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
