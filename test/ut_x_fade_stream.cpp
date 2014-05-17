/*
 * Unit Test XFadeStream
 */
#include "assert.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "x_fade_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

#define EPSILON 0.0001

bool near(double f1, double f2) {
  double d = EPSILON;
  bool pass = ((f1 - d) < f2) & ((f1 + d) > f2);
  if (pass == false) {
    fprintf(stderr,"near: %f should be near %f\n", f1, f2);
  }
  return pass;
}

int main() {
  mu::ConstantStream constant_stream;
  mu::CropStream crop_stream;
  mu::XFadeStream x_fade_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  constant_stream.setValue(1.0);
  crop_stream.setSource(&constant_stream).setStart(100).setEnd(200);

  // ================================================================
  fprintf(stderr, "=== null source\n");
  x_fade_stream.setSource(NULL);
  x_fade_stream.setStart(mu::kIndefinite).setEnd(mu::kIndefinite);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(x_fade_stream.getStart()==mu::kIndefinite);
  ASSERT(x_fade_stream.getEnd()==mu::kIndefinite);

  // ================================================================
  // When source has indefinite extent, fade in and fade out times are
  // determinined by fade_stream->setStart() and setEnd().
  fprintf(stderr, "=== constant source, start=kIndefinite, end=kIndefinite\n");
  x_fade_stream.setSource(&constant_stream);
  x_fade_stream.setStart(mu::kIndefinite).setEnd(mu::kIndefinite);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(buffer(0,0) == 1.0);
  ASSERT(buffer(0,1) == 1.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 1.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 1.0);

  ASSERT(x_fade_stream.getStart()==mu::kIndefinite);
  ASSERT(x_fade_stream.getEnd()==mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== constant source, start=100, end=200\n");
  x_fade_stream.setSource(&constant_stream);
  x_fade_stream.setStart(100).setEnd(200).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.1));
  ASSERT(near(buffer(96,1), 0.1));
  ASSERT(near(buffer(97,0), 0.2));
  ASSERT(near(buffer(97,1), 0.2));
  ASSERT(near(buffer(98,0), 0.3));
  ASSERT(near(buffer(98,1), 0.3));
  ASSERT(near(buffer(99,0), 0.4));
  ASSERT(near(buffer(99,1), 0.4));  
  ASSERT(near(buffer(100,0), 0.5));
  ASSERT(near(buffer(100,1), 0.5));
  ASSERT(near(buffer(101,0), 0.6));
  ASSERT(near(buffer(101,1), 0.6));
  ASSERT(near(buffer(102,0), 0.7));
  ASSERT(near(buffer(102,1), 0.7));
  ASSERT(near(buffer(103,0), 0.8));
  ASSERT(near(buffer(103,1), 0.8));
  ASSERT(near(buffer(104,0), 0.9));
  ASSERT(near(buffer(104,1), 0.9));
  ASSERT(near(buffer(105,0), 1.0));
  ASSERT(near(buffer(105,1), 1.0));

  ASSERT(near(buffer(195,0), 1.0));
  ASSERT(near(buffer(195,1), 1.0));
  ASSERT(near(buffer(196,0), 0.9));
  ASSERT(near(buffer(196,1), 0.9));
  ASSERT(near(buffer(197,0), 0.8));
  ASSERT(near(buffer(197,1), 0.8));
  ASSERT(near(buffer(198,0), 0.7));
  ASSERT(near(buffer(198,1), 0.7));
  ASSERT(near(buffer(199,0), 0.6));
  ASSERT(near(buffer(199,1), 0.6));  
  ASSERT(near(buffer(200,0), 0.5));
  ASSERT(near(buffer(200,1), 0.5));
  ASSERT(near(buffer(201,0), 0.4));
  ASSERT(near(buffer(201,1), 0.4));
  ASSERT(near(buffer(202,0), 0.3));
  ASSERT(near(buffer(202,1), 0.3));
  ASSERT(near(buffer(203,0), 0.2));
  ASSERT(near(buffer(203,1), 0.2));
  ASSERT(near(buffer(204,0), 0.1));
  ASSERT(near(buffer(204,1), 0.1));
  ASSERT(near(buffer(205,0), 0.0));
  ASSERT(near(buffer(205,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==95);
  ASSERT(x_fade_stream.getEnd()==205);

  // ================================================================
  fprintf(stderr, "=== constant source, start=100, end=kIndefinite\n");
  x_fade_stream.setSource(&constant_stream);
  x_fade_stream.setStart(100).setEnd(mu::kIndefinite).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.1));
  ASSERT(near(buffer(96,1), 0.1));
  ASSERT(near(buffer(97,0), 0.2));
  ASSERT(near(buffer(97,1), 0.2));
  ASSERT(near(buffer(98,0), 0.3));
  ASSERT(near(buffer(98,1), 0.3));
  ASSERT(near(buffer(99,0), 0.4));
  ASSERT(near(buffer(99,1), 0.4));  
  ASSERT(near(buffer(100,0), 0.5));
  ASSERT(near(buffer(100,1), 0.5));
  ASSERT(near(buffer(101,0), 0.6));
  ASSERT(near(buffer(101,1), 0.6));
  ASSERT(near(buffer(102,0), 0.7));
  ASSERT(near(buffer(102,1), 0.7));
  ASSERT(near(buffer(103,0), 0.8));
  ASSERT(near(buffer(103,1), 0.8));
  ASSERT(near(buffer(104,0), 0.9));
  ASSERT(near(buffer(104,1), 0.9));
  ASSERT(near(buffer(105,0), 1.0));
  ASSERT(near(buffer(105,1), 1.0));

  ASSERT(near(buffer(195,0), 1.0));
  ASSERT(near(buffer(195,1), 1.0));
  ASSERT(near(buffer(196,0), 1.0));
  ASSERT(near(buffer(196,1), 1.0));
  ASSERT(near(buffer(197,0), 1.0));
  ASSERT(near(buffer(197,1), 1.0));
  ASSERT(near(buffer(198,0), 1.0));
  ASSERT(near(buffer(198,1), 1.0));
  ASSERT(near(buffer(199,0), 1.0));
  ASSERT(near(buffer(199,1), 1.0));  
  ASSERT(near(buffer(200,0), 1.0));
  ASSERT(near(buffer(200,1), 1.0));
  ASSERT(near(buffer(201,0), 1.0));
  ASSERT(near(buffer(201,1), 1.0));
  ASSERT(near(buffer(202,0), 1.0));
  ASSERT(near(buffer(202,1), 1.0));
  ASSERT(near(buffer(203,0), 1.0));
  ASSERT(near(buffer(203,1), 1.0));
  ASSERT(near(buffer(204,0), 1.0));
  ASSERT(near(buffer(204,1), 1.0));
  ASSERT(near(buffer(205,0), 1.0));
  ASSERT(near(buffer(205,1), 1.0));

  ASSERT(buffer((FRAME_COUNT-1),0) == 1.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 1.0);

  ASSERT(x_fade_stream.getStart()==95);
  ASSERT(x_fade_stream.getEnd()==mu::kIndefinite);

  // ================================================================
  fprintf(stderr, "=== constant source, start=kIndefinite, end=200\n");
  x_fade_stream.setSource(&constant_stream);
  x_fade_stream.setStart(mu::kIndefinite).setEnd(200).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 1.0));
  ASSERT(near(buffer(95,1), 1.0));
  ASSERT(near(buffer(96,0), 1.0));
  ASSERT(near(buffer(96,1), 1.0));
  ASSERT(near(buffer(97,0), 1.0));
  ASSERT(near(buffer(97,1), 1.0));
  ASSERT(near(buffer(98,0), 1.0));
  ASSERT(near(buffer(98,1), 1.0));
  ASSERT(near(buffer(99,0), 1.0));
  ASSERT(near(buffer(99,1), 1.0));
  ASSERT(near(buffer(100,0), 1.0));
  ASSERT(near(buffer(100,1), 1.0));
  ASSERT(near(buffer(101,0), 1.0));
  ASSERT(near(buffer(101,1), 1.0));
  ASSERT(near(buffer(102,0), 1.0));
  ASSERT(near(buffer(102,1), 1.0));
  ASSERT(near(buffer(103,0), 1.0));
  ASSERT(near(buffer(103,1), 1.0));
  ASSERT(near(buffer(104,0), 1.0));
  ASSERT(near(buffer(104,1), 1.0));
  ASSERT(near(buffer(105,0), 1.0));
  ASSERT(near(buffer(105,1), 1.0));

  ASSERT(near(buffer(195,0), 1.0));
  ASSERT(near(buffer(195,1), 1.0));
  ASSERT(near(buffer(196,0), 0.9));
  ASSERT(near(buffer(196,1), 0.9));
  ASSERT(near(buffer(197,0), 0.8));
  ASSERT(near(buffer(197,1), 0.8));
  ASSERT(near(buffer(198,0), 0.7));
  ASSERT(near(buffer(198,1), 0.7));
  ASSERT(near(buffer(199,0), 0.6));
  ASSERT(near(buffer(199,1), 0.6));  
  ASSERT(near(buffer(200,0), 0.5));
  ASSERT(near(buffer(200,1), 0.5));
  ASSERT(near(buffer(201,0), 0.4));
  ASSERT(near(buffer(201,1), 0.4));
  ASSERT(near(buffer(202,0), 0.3));
  ASSERT(near(buffer(202,1), 0.3));
  ASSERT(near(buffer(203,0), 0.2));
  ASSERT(near(buffer(203,1), 0.2));
  ASSERT(near(buffer(204,0), 0.1));
  ASSERT(near(buffer(204,1), 0.1));
  ASSERT(near(buffer(205,0), 0.0));
  ASSERT(near(buffer(205,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==mu::kIndefinite);
  ASSERT(x_fade_stream.getEnd()==205);

  // ================================================================
  // NB: when source has definite extent, fade in starts at
  // source->getStart() and ends at source->getEnd().
  fprintf(stderr, "=== cropped source, start=kIndefinite, end=kIndefinite\n");
  x_fade_stream.setSource(&crop_stream);
  x_fade_stream.setStart(mu::kIndefinite).setEnd(mu::kIndefinite).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.0));
  ASSERT(near(buffer(96,1), 0.0));
  ASSERT(near(buffer(97,0), 0.0));
  ASSERT(near(buffer(97,1), 0.0));
  ASSERT(near(buffer(98,0), 0.0));
  ASSERT(near(buffer(98,1), 0.0));
  ASSERT(near(buffer(99,0), 0.0));
  ASSERT(near(buffer(99,1), 0.0));  
  ASSERT(near(buffer(100,0), 0.0));
  ASSERT(near(buffer(100,1), 0.0));
  ASSERT(near(buffer(101,0), 0.1));
  ASSERT(near(buffer(101,1), 0.1));
  ASSERT(near(buffer(102,0), 0.2));
  ASSERT(near(buffer(102,1), 0.2));
  ASSERT(near(buffer(103,0), 0.3));
  ASSERT(near(buffer(103,1), 0.3));
  ASSERT(near(buffer(104,0), 0.4));
  ASSERT(near(buffer(104,1), 0.4));
  ASSERT(near(buffer(105,0), 0.5));
  ASSERT(near(buffer(105,1), 0.5));
  ASSERT(near(buffer(106,0), 0.6));
  ASSERT(near(buffer(106,1), 0.6));
  ASSERT(near(buffer(107,0), 0.7));
  ASSERT(near(buffer(107,1), 0.7));
  ASSERT(near(buffer(108,0), 0.8));
  ASSERT(near(buffer(108,1), 0.8));
  ASSERT(near(buffer(109,0), 0.9));
  ASSERT(near(buffer(109,1), 0.9));
  ASSERT(near(buffer(110,0), 1.0));
  ASSERT(near(buffer(110,1), 1.0));

  ASSERT(near(buffer(190,0), 1.0));
  ASSERT(near(buffer(190,1), 1.0));
  ASSERT(near(buffer(191,0), 0.9));
  ASSERT(near(buffer(191,1), 0.9));
  ASSERT(near(buffer(192,0), 0.8));
  ASSERT(near(buffer(192,1), 0.8));
  ASSERT(near(buffer(193,0), 0.7));
  ASSERT(near(buffer(193,1), 0.7));
  ASSERT(near(buffer(194,0), 0.6));
  ASSERT(near(buffer(194,1), 0.6));  
  ASSERT(near(buffer(195,0), 0.5));
  ASSERT(near(buffer(195,1), 0.5));
  ASSERT(near(buffer(196,0), 0.4));
  ASSERT(near(buffer(196,1), 0.4));
  ASSERT(near(buffer(197,0), 0.3));
  ASSERT(near(buffer(197,1), 0.3));
  ASSERT(near(buffer(198,0), 0.2));
  ASSERT(near(buffer(198,1), 0.2));
  ASSERT(near(buffer(199,0), 0.1));
  ASSERT(near(buffer(199,1), 0.1));
  ASSERT(near(buffer(200,0), 0.0));
  ASSERT(near(buffer(200,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==100);
  ASSERT(x_fade_stream.getEnd()==200);

  // ================================================================
  fprintf(stderr, "=== cropped source, start=100, end=200\n");
  x_fade_stream.setSource(&crop_stream);
  x_fade_stream.setStart(100).setEnd(200).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.0));
  ASSERT(near(buffer(96,1), 0.0));
  ASSERT(near(buffer(97,0), 0.0));
  ASSERT(near(buffer(97,1), 0.0));
  ASSERT(near(buffer(98,0), 0.0));
  ASSERT(near(buffer(98,1), 0.0));
  ASSERT(near(buffer(99,0), 0.0));
  ASSERT(near(buffer(99,1), 0.0));  
  ASSERT(near(buffer(100,0), 0.0));
  ASSERT(near(buffer(100,1), 0.0));
  ASSERT(near(buffer(101,0), 0.1));
  ASSERT(near(buffer(101,1), 0.1));
  ASSERT(near(buffer(102,0), 0.2));
  ASSERT(near(buffer(102,1), 0.2));
  ASSERT(near(buffer(103,0), 0.3));
  ASSERT(near(buffer(103,1), 0.3));
  ASSERT(near(buffer(104,0), 0.4));
  ASSERT(near(buffer(104,1), 0.4));
  ASSERT(near(buffer(105,0), 0.5));
  ASSERT(near(buffer(105,1), 0.5));
  ASSERT(near(buffer(106,0), 0.6));
  ASSERT(near(buffer(106,1), 0.6));
  ASSERT(near(buffer(107,0), 0.7));
  ASSERT(near(buffer(107,1), 0.7));
  ASSERT(near(buffer(108,0), 0.8));
  ASSERT(near(buffer(108,1), 0.8));
  ASSERT(near(buffer(109,0), 0.9));
  ASSERT(near(buffer(109,1), 0.9));
  ASSERT(near(buffer(110,0), 1.0));
  ASSERT(near(buffer(110,1), 1.0));

  ASSERT(near(buffer(190,0), 1.0));
  ASSERT(near(buffer(190,1), 1.0));
  ASSERT(near(buffer(191,0), 0.9));
  ASSERT(near(buffer(191,1), 0.9));
  ASSERT(near(buffer(192,0), 0.8));
  ASSERT(near(buffer(192,1), 0.8));
  ASSERT(near(buffer(193,0), 0.7));
  ASSERT(near(buffer(193,1), 0.7));
  ASSERT(near(buffer(194,0), 0.6));
  ASSERT(near(buffer(194,1), 0.6));  
  ASSERT(near(buffer(195,0), 0.5));
  ASSERT(near(buffer(195,1), 0.5));
  ASSERT(near(buffer(196,0), 0.4));
  ASSERT(near(buffer(196,1), 0.4));
  ASSERT(near(buffer(197,0), 0.3));
  ASSERT(near(buffer(197,1), 0.3));
  ASSERT(near(buffer(198,0), 0.2));
  ASSERT(near(buffer(198,1), 0.2));
  ASSERT(near(buffer(199,0), 0.1));
  ASSERT(near(buffer(199,1), 0.1));
  ASSERT(near(buffer(200,0), 0.0));
  ASSERT(near(buffer(200,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==100);
  ASSERT(x_fade_stream.getEnd()==200);

  // ================================================================
  // When both source stream and fade stream have definite extent, the
  // start time is 
  // max(source->getStart(),fader->getStart()+(fader->getXFadeTime()/2))
  // and end time is
  // min(source->getEnd(),fader->getEnd()-(fader->getXFadeTime()/2))
  //
  fprintf(stderr, "=== cropped source, start=105, end=195\n");
  x_fade_stream.setSource(&crop_stream);
  x_fade_stream.setStart(105).setEnd(195).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.0));
  ASSERT(near(buffer(96,1), 0.0));
  ASSERT(near(buffer(97,0), 0.0));
  ASSERT(near(buffer(97,1), 0.0));
  ASSERT(near(buffer(98,0), 0.0));
  ASSERT(near(buffer(98,1), 0.0));
  ASSERT(near(buffer(99,0), 0.0));
  ASSERT(near(buffer(99,1), 0.0));  
  ASSERT(near(buffer(100,0), 0.0));
  ASSERT(near(buffer(100,1), 0.0));
  ASSERT(near(buffer(101,0), 0.1));
  ASSERT(near(buffer(101,1), 0.1));
  ASSERT(near(buffer(102,0), 0.2));
  ASSERT(near(buffer(102,1), 0.2));
  ASSERT(near(buffer(103,0), 0.3));
  ASSERT(near(buffer(103,1), 0.3));
  ASSERT(near(buffer(104,0), 0.4));
  ASSERT(near(buffer(104,1), 0.4));
  ASSERT(near(buffer(105,0), 0.5));
  ASSERT(near(buffer(105,1), 0.5));
  ASSERT(near(buffer(106,0), 0.6));
  ASSERT(near(buffer(106,1), 0.6));
  ASSERT(near(buffer(107,0), 0.7));
  ASSERT(near(buffer(107,1), 0.7));
  ASSERT(near(buffer(108,0), 0.8));
  ASSERT(near(buffer(108,1), 0.8));
  ASSERT(near(buffer(109,0), 0.9));
  ASSERT(near(buffer(109,1), 0.9));
  ASSERT(near(buffer(110,0), 1.0));
  ASSERT(near(buffer(110,1), 1.0));

  ASSERT(near(buffer(190,0), 1.0));
  ASSERT(near(buffer(190,1), 1.0));
  ASSERT(near(buffer(191,0), 0.9));
  ASSERT(near(buffer(191,1), 0.9));
  ASSERT(near(buffer(192,0), 0.8));
  ASSERT(near(buffer(192,1), 0.8));
  ASSERT(near(buffer(193,0), 0.7));
  ASSERT(near(buffer(193,1), 0.7));
  ASSERT(near(buffer(194,0), 0.6));
  ASSERT(near(buffer(194,1), 0.6));  
  ASSERT(near(buffer(195,0), 0.5));
  ASSERT(near(buffer(195,1), 0.5));
  ASSERT(near(buffer(196,0), 0.4));
  ASSERT(near(buffer(196,1), 0.4));
  ASSERT(near(buffer(197,0), 0.3));
  ASSERT(near(buffer(197,1), 0.3));
  ASSERT(near(buffer(198,0), 0.2));
  ASSERT(near(buffer(198,1), 0.2));
  ASSERT(near(buffer(199,0), 0.1));
  ASSERT(near(buffer(199,1), 0.1));
  ASSERT(near(buffer(200,0), 0.0));
  ASSERT(near(buffer(200,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==100);
  ASSERT(x_fade_stream.getEnd()==200);

  // ================================================================
  // When both source stream and fade stream have definite extent, the
  // start time is 
  // max(source->getStart(),fader->getStart()+(fader->getXFadeTime()/2))
  // and end time is
  // min(source->getEnd(),fader->getEnd()-(fader->getXFadeTime()/2))
  //
  fprintf(stderr, "=== cropped source, start=110, end=190\n");
  x_fade_stream.setSource(&crop_stream);
  x_fade_stream.setStart(110).setEnd(190).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(95,0), 0.0));
  ASSERT(near(buffer(95,1), 0.0));
  ASSERT(near(buffer(96,0), 0.0));
  ASSERT(near(buffer(96,1), 0.0));
  ASSERT(near(buffer(97,0), 0.0));
  ASSERT(near(buffer(97,1), 0.0));
  ASSERT(near(buffer(98,0), 0.0));
  ASSERT(near(buffer(98,1), 0.0));
  ASSERT(near(buffer(99,0), 0.0));
  ASSERT(near(buffer(99,1), 0.0));  
  ASSERT(near(buffer(100,0), 0.0));
  ASSERT(near(buffer(100,1), 0.0));
  ASSERT(near(buffer(101,0), 0.0));
  ASSERT(near(buffer(101,1), 0.0));
  ASSERT(near(buffer(102,0), 0.0));
  ASSERT(near(buffer(102,1), 0.0));
  ASSERT(near(buffer(103,0), 0.0));
  ASSERT(near(buffer(103,1), 0.0));
  ASSERT(near(buffer(104,0), 0.0));
  ASSERT(near(buffer(104,1), 0.0));
  ASSERT(near(buffer(105,0), 0.0));
  ASSERT(near(buffer(105,1), 0.0));
  ASSERT(near(buffer(106,0), 0.1));
  ASSERT(near(buffer(106,1), 0.1));
  ASSERT(near(buffer(107,0), 0.2));
  ASSERT(near(buffer(107,1), 0.2));
  ASSERT(near(buffer(108,0), 0.3));
  ASSERT(near(buffer(108,1), 0.3));
  ASSERT(near(buffer(109,0), 0.4));
  ASSERT(near(buffer(109,1), 0.4));
  ASSERT(near(buffer(110,0), 0.5));
  ASSERT(near(buffer(110,1), 0.5));
  ASSERT(near(buffer(111,0), 0.6));
  ASSERT(near(buffer(111,1), 0.6));
  ASSERT(near(buffer(112,0), 0.7));
  ASSERT(near(buffer(112,1), 0.7));
  ASSERT(near(buffer(113,0), 0.8));
  ASSERT(near(buffer(113,1), 0.8));
  ASSERT(near(buffer(114,0), 0.9));
  ASSERT(near(buffer(114,1), 0.9));
  ASSERT(near(buffer(115,0), 1.0));
  ASSERT(near(buffer(115,1), 1.0));

  ASSERT(near(buffer(185,0), 1.0));
  ASSERT(near(buffer(185,1), 1.0));
  ASSERT(near(buffer(186,0), 0.9));
  ASSERT(near(buffer(186,1), 0.9));
  ASSERT(near(buffer(187,0), 0.8));
  ASSERT(near(buffer(187,1), 0.8));
  ASSERT(near(buffer(188,0), 0.7));
  ASSERT(near(buffer(188,1), 0.7));
  ASSERT(near(buffer(189,0), 0.6));
  ASSERT(near(buffer(189,1), 0.6));
  ASSERT(near(buffer(190,0), 0.5));
  ASSERT(near(buffer(190,1), 0.5));
  ASSERT(near(buffer(191,0), 0.4));
  ASSERT(near(buffer(191,1), 0.4));
  ASSERT(near(buffer(192,0), 0.3));
  ASSERT(near(buffer(192,1), 0.3));
  ASSERT(near(buffer(193,0), 0.2));
  ASSERT(near(buffer(193,1), 0.2));
  ASSERT(near(buffer(194,0), 0.1));
  ASSERT(near(buffer(194,1), 0.1));  
  ASSERT(near(buffer(195,0), 0.0));
  ASSERT(near(buffer(195,1), 0.0));
  ASSERT(near(buffer(196,0), 0.0));
  ASSERT(near(buffer(196,1), 0.0));
  ASSERT(near(buffer(197,0), 0.0));
  ASSERT(near(buffer(197,1), 0.0));
  ASSERT(near(buffer(198,0), 0.0));
  ASSERT(near(buffer(198,1), 0.0));
  ASSERT(near(buffer(199,0), 0.0));
  ASSERT(near(buffer(199,1), 0.0));
  ASSERT(near(buffer(200,0), 0.0));
  ASSERT(near(buffer(200,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==105);
  ASSERT(x_fade_stream.getEnd()==195);

  // ================================================================
  // When start and end times are close together, it fades in and fades
  // out without reaching unity.
  fprintf(stderr, "=== cropped source, start=150, end=150\n");
  x_fade_stream.setSource(&crop_stream);
  x_fade_stream.setStart(150).setEnd(150).setXFadeTime(10);

  x_fade_stream.step(buffer, 0, player);

  ASSERT(near(buffer(140,0), 0.0));
  ASSERT(near(buffer(140,1), 0.0));
  ASSERT(near(buffer(141,0), 0.0));
  ASSERT(near(buffer(141,1), 0.0));
  ASSERT(near(buffer(142,0), 0.0));
  ASSERT(near(buffer(142,1), 0.0));
  ASSERT(near(buffer(143,0), 0.0));
  ASSERT(near(buffer(143,1), 0.0));
  ASSERT(near(buffer(144,0), 0.0));
  ASSERT(near(buffer(144,1), 0.0));
  ASSERT(near(buffer(145,0), 0.0));
  ASSERT(near(buffer(145,1), 0.0));
  ASSERT(near(buffer(146,0), 0.1));
  ASSERT(near(buffer(146,1), 0.1));
  ASSERT(near(buffer(147,0), 0.2));
  ASSERT(near(buffer(147,1), 0.2));
  ASSERT(near(buffer(148,0), 0.3));
  ASSERT(near(buffer(148,1), 0.3));
  ASSERT(near(buffer(149,0), 0.4));
  ASSERT(near(buffer(149,1), 0.4));
  ASSERT(near(buffer(150,0), 0.5));
  ASSERT(near(buffer(150,1), 0.5));
  ASSERT(near(buffer(151,0), 0.4));
  ASSERT(near(buffer(151,1), 0.4));
  ASSERT(near(buffer(152,0), 0.3));
  ASSERT(near(buffer(152,1), 0.3));
  ASSERT(near(buffer(153,0), 0.2));
  ASSERT(near(buffer(153,1), 0.2));
  ASSERT(near(buffer(154,0), 0.1));
  ASSERT(near(buffer(154,1), 0.1));
  ASSERT(near(buffer(155,0), 0.0));
  ASSERT(near(buffer(155,1), 0.0));
  ASSERT(near(buffer(156,0), 0.0));
  ASSERT(near(buffer(156,1), 0.0));

  ASSERT(near(buffer((FRAME_COUNT-1),0), 0.0));
  ASSERT(near(buffer((FRAME_COUNT-1),1), 0.0));

  ASSERT(x_fade_stream.getStart()==145);
  ASSERT(x_fade_stream.getEnd()==155);

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
