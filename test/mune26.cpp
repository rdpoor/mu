/*
 * Test FadeStream with pure sine tones.
 */
#include "mu.h"

#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "fade_stream.h"
#include "file_write_stream.h"
#include "sine_stream.h"
#include "nrt_player.h"
#include "rt_player.h"
#include <map>
#include <unistd.h>

#define TQ (44100)
#define TE (TQ/2)
#define TS (TQ/4)
#define TT (TQ/8)

mu::Stream *makeNote(mu::Tick start, mu::Tick duration, stk::StkFloat pitch) {
  mu::SineStream *ss = &(new mu::SineStream())->setPitch(pitch).setAmplitude(0.25);
#if 1
  mu::FadeStream *fs = &(new mu::FadeStream())->
    setStart(start).
    setEnd(start + duration).
    setFadeTime(TS).
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
#if 0
  mu::RtPlayer player;
  // mu::NrtPlayer player;
  mu::AddStream as;
  mu::FileWriteStream fws;

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
  mu::FadeStream fs;
  mu::FileWriteStream fws;
  mu::NrtPlayer player;
  mu::SineStream ss;

#define FT (44100/4)

  ss.setPitch(69).setAmplitude(0.25);
  fs.setSource(&ss).setStart(FT).setEnd(44100+FT).setFadeTime(FT);
  fws.setFileName("mune26.wav").setSource(&fs);
  player.setSource(&fws);

  fprintf(stderr, "fade_stream getStart()=%ld, getEnd()=%ld\n", fs.getStart(), fs.getEnd());
#endif

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();

  //  std::cout << player.getSource()->inspect();

  return 0;
}
