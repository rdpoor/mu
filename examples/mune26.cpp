/*
 * Test FadeSP with pure sine tones.
 */
#include "mu.h"

#include "add_sp.h"
#include "crop_sp.h"
#include "delay_sp.h"
#include "fade_sp.h"
#include "file_write_sp.h"
#include "sine_sp.h"
#include "nrt_player.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define TQ (44100)
#define TE (TQ/2)
#define TS (TQ/4)
#define TT (TQ/8)

mu::SampleProcessor *makeNote(mu::Tick start, mu::Tick duration, stk::StkFloat pitch) {
  mu::SineSP *ss = &(new mu::SineSP())->setPitch(pitch).setAmplitude(0.25);
#if 1
  mu::FadeSP *fs = &(new mu::FadeSP())->
    setStart(start).
    setEnd(start + duration).
    setFadeTime(TS).
    setSource(ss);
  return fs;
#else
  mu::CropSP *cs = &(new mu::CropSP())->
    setStart(start).
    setEnd(start+duration).
    setSource(ss);
  return cs;
#endif
}

// ================================================================
int main() {
#if 0
  mu::RtPlayer player;
  // mu::NrtPlayer player;
  mu::AddSP as;
  mu::FileWriteSP fws;

  mu::Tick t;
  t = 0;
  for (int pitch=60; pitch<=83; pitch++) {
    bool is_last = pitch == 83;
    as.addSource(makeNote(t, is_last ? TQ : TE, pitch));
    t += TE;
  }

  fws.setFileName("mune26.wav").setSource(&as);
  player.setSource(&fws);
#else
  mu::FadeSP fs;
  mu::FileWriteSP fws;
  mu::NrtPlayer player;
  mu::SineSP ss;

#define FT (44100/4)

  ss.setPitch(69).setAmplitude(0.25);
  fs.setSource(&ss).setStart(FT).setEnd(44100+FT).setFadeTime(FT);
  fws.setFileName("mune26.wav").setSource(&fs);
  player.setSource(&fws);

  fprintf(stderr, "fade_sp getStart()=%ld, getEnd()=%ld\n", fs.getStart(), fs.getEnd());
#endif

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
