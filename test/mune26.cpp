/*
 * Test FadeStream with pure sine tones.
 */
#include "mu.h"

#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "sine_stream.h"
#include "nrt_player.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

mu::Stream *makeNote(mu::Tick start, mu::Tick duration, stk::StkFloat pitch) {
  mu::SineStream *ss = &(new mu::SineStream())->setPitch(pitch).setAmplitude(0.25);
#if 0
  mu::FadeStream *fs = &(new mu::FadeStream())->
    setStart(start).
    setEnd(start + duration).
    setFadeTime(200).
    setSource(ss);
  return fs;
#else
  mu::CropStream *cs = &(new mu::CropStream())->
    setStart(start).
    setEnd(start+duration).
    setSource(ss);
  return cs;
#endif
}

// ================================================================
int main() {
  mu::RtPlayer player;
  // mu::NrtPlayer player;
  mu::AddStream as;

#define TQ (44100 * 0.51)
#define TE (TQ/2)
#define TS (TQ/4)
#define TT (TQ/8)

  mu::Tick t;
  t = 0;
  for (int pitch=60; pitch<=83; pitch++) {
    bool is_last = pitch == 83;
    as.addSource(makeNote(t, is_last ? 44100 : TT, pitch));
    t += TT;
  }

  player.setSource(&as);
  player.start();
  // fprintf(stderr, "Type [return] to quit:"); getchar();
  sleep(5);
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
