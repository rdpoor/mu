/*
 * Make some fun and varying drum pattern using mu::ProbabilityStream
 */
#include "add_stream.h"
#include "delay_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include "probability_stream.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

#define TEMPO_BPM 150
#define BEAT_DURATION_TICS ((44100 * 60) / (TEMPO_BPM))

int max(std::vector<int> v) {
  int s = 1;
  for (size_t i=0; i<v.size(); i++) { if (v.at(i) > s) s = v.at(i); }
  return s;
}

/*
 * Create a stream that loops every \c duration_in_beats.  At the start of
 * each loop, assign probabilities that \c file_name will pay on the 
 * i'th sub-beat defined by \c probabilities.
 */
mu::Stream *make_probabilty_stream(std::string file_name, 
                                   std::vector<int> probabilities,
                                   int duration_in_beats = 4) {
  
  int p_max = max(probabilities);
  // can we share frs among multiple streams?
  mu::FileReadStream *frs = &(new mu::FileReadStream())->fileName(file_name).doNormalize(true);
  mu::AddStream *as = new mu::AddStream();
  for (long int i=0; i<probabilities.size(); i++) {
    mu::Tick delay = BEAT_DURATION_TICS * duration_in_beats * i / probabilities.size();
    double probability = (double)probabilities.at(i) / (double)p_max;
    mu::DelayStream *ds = &(new mu::DelayStream())->setDelay(delay).setSource(frs);
    mu::ProbabilityStream *ps = &(new mu::ProbabilityStream())->setSource(ds).setProbability(probability);
    as->addSource(ps);
  }
  mu::LoopStream *ls = &(new mu::LoopStream())->
    setLoopDuration(duration_in_beats * BEAT_DURATION_TICS).
    setSource(as);
  return ls;
}


int main() {
  mu::AddStream add_stream;
  mu::RtPlayer player;

  printf("beat duration tics = %d\n", BEAT_DURATION_TICS);

  static const int p0[] = {4, 0, 0, 0, 0, 0, 0, 1};
  static const int p1[] = {1, 8, 1, 8};
  static const int p2[] = {1, 2, 1, 2, 1, 2, 1, 3};
  static const int p3[] = {3, 1, 2, 3, 1, 2, 3, 0, 3, 1, 2, 1, 1, 2, 3, 1};

  add_stream.addSource(make_probabilty_stream(SOUND_DIR "s15.wav",
                                              std::vector<int>(p0, p0 + sizeof(p0) / sizeof(int))));
  add_stream.addSource(make_probabilty_stream(SOUND_DIR "s02.wav",
                                              std::vector<int>(p1, p1 + sizeof(p1) / sizeof(int))));
  add_stream.addSource(make_probabilty_stream(SOUND_DIR "s12.wav",
                                              std::vector<int>(p2, p2 + sizeof(p2) / sizeof(int))));
  add_stream.addSource(make_probabilty_stream(SOUND_DIR "s11.wav",
                                              std::vector<int>(p3, p3 + sizeof(p3) / sizeof(int))));

  player.setSource(&add_stream);

  player.start();
  fprintf(stderr, "Type [return] to quit:");
  getchar();
  player.stop();

  return 0;
}
