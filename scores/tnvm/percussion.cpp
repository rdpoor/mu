/*
 * Together Now, Very Minor.  Percussion Part.
 */
#include "percussion.h"

#define SOUND_DIR "/Users/r/Projects/Mu/SoundSets/TAS/"

/*
 * specialized maximum fn that returns the largest value in a vector in ints.
 */
int max(std::vector<int> v) {
  int s = 1;
  for (size_t i=0; i<v.size(); i++) { if (v.at(i) > s) s = v.at(i); }
  return s;
}

static mu::Stream *make_probabilty_stream(std::string file_name, 
                                          std::vector<int> probabilities,
                                          mu::Tick start_frame,
                                          mu::Tick end_frame,
                                          int loop_duration_in_beats = 4) {
  
  int p_max = max(probabilities);
  // can we share frs among multiple streams?
  mu::FileReadStream *frs = &(new mu::FileReadStream())->fileName(file_name).doNormalize(true);
  mu::ChannelizerStream *chs = &(new mu::ChannelizerStream())->setSourceChannelCount(1).setSource(frs);
  mu::AddStream *as = new mu::AddStream();
  for (long int i=0; i<probabilities.size(); i++) {
    mu::Tick delay = BEAT_TO_FRAME(loop_duration_in_beats) * i / probabilities.size();
    double probability = (double)probabilities.at(i) / (double)p_max;
    mu::DelayStream *ds = &(new mu::DelayStream())->setDelay(delay).setSource(chs);
    mu::ProbabilityStream *ps = &(new mu::ProbabilityStream())->setSource(ds).setProbability(probability);
    as->addSource(ps);
  }
  mu::LoopStream *ls = &(new mu::LoopStream())->
    setLoopDuration(BEAT_TO_FRAME(loop_duration_in_beats)).
    setSource(as);
  mu::CropStream *cs = &(new mu::CropStream())->setSource(ls).setStart(start_frame).setEnd(end_frame);
  return cs;
}

// s00.wav: shhh
// s01.wav: sh
// s02.wav: bop
// s03.wav: clickle
// s04.wav: clickle!afplay
// s05.wav: tink
// s06.wav: whunk
// s07.wav: thunk
// s08.wav: kascrape
// s09.wav: wshhhh
// s10.wav: zzzztsch
// s11.wav: tup
// s12.wav: Tup
// s13.wav: kluck
// s14.wav: tic/thmp
// s15.wav: bmp
// s16.wav: bmp
mu::Stream *make_percussion_stream() {
  mu::AddStream *as = new mu::AddStream();
  
  static const int p0[] = {7, 0, 0, 3, 0, 0, 0, 0, 7, 0, 0, 3, 0, 0, 0, 1};

  as->addSource(make_probabilty_stream(SOUND_DIR "s12.wav",
                                       std::vector<int>(p0, p0 + sizeof(p0) / sizeof(int)),
                                       MEASURE_TO_FRAME(M_INTRO),
                                       MEASURE_TO_FRAME(M_END),
                                       8));
  return as;
}
