/*
 * Unit Test FadeSP
 */
#include "assert.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include "fade_sp.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

int main() {
  mu::ConstantSP constant_sp;
  mu::CropSP crop_sp;
  mu::FadeSP fade_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  constant_sp.setValue(1.0);
  crop_sp.setSource(&constant_sp).setStart(100).setEnd(200);

  // ================================================================
  fprintf(stderr, "=== null source\n");
  fade_sp.setSource(NULL);
  fade_sp.setStart(mu::TickUtils::indefinite()).setEnd(mu::TickUtils::indefinite());

  fade_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 0);
  ASSERT(buffer(0,1) == 0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 0);

  ASSERT(fade_sp.getStart()==mu::TickUtils::indefinite());
  ASSERT(fade_sp.getEnd()==mu::TickUtils::indefinite());

  // ================================================================
  // When source has indefinite extent, fade in and fade out times are
  // determinined by fade_sp->setStart() and setEnd().
  fprintf(stderr, "=== constant source, start=TickUtils::indefinite(), end=TickUtils::indefinite()\n");
  fade_sp.setSource(&constant_sp);
  fade_sp.setStart(mu::TickUtils::indefinite()).setEnd(mu::TickUtils::indefinite());

  fade_sp.step(buffer, 0, true);

  ASSERT(buffer(0,0) == 1.0);
  ASSERT(buffer(0,1) == 1.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 1.0);
  ASSERT(buffer((FRAME_COUNT-1),1) == 1.0);

  ASSERT(fade_sp.getStart()==mu::TickUtils::indefinite());
  ASSERT(fade_sp.getEnd()==mu::TickUtils::indefinite());

  // ================================================================
  fprintf(stderr, "=== constant source, start=100, end=200\n");
  fade_sp.setSource(&constant_sp);
  fade_sp.setStart(100).setEnd(200).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==95);
  ASSERT(fade_sp.getEnd()==205);

  // ================================================================
  fprintf(stderr, "=== constant source, start=100, end=TickUtils::indefinite()\n");
  fade_sp.setSource(&constant_sp);
  fade_sp.setStart(100).setEnd(mu::TickUtils::indefinite()).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==95);
  ASSERT(fade_sp.getEnd()==mu::TickUtils::indefinite());

  // ================================================================
  fprintf(stderr, "=== constant source, start=TickUtils::indefinite(), end=200\n");
  fade_sp.setSource(&constant_sp);
  fade_sp.setStart(mu::TickUtils::indefinite()).setEnd(200).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==mu::TickUtils::indefinite());
  ASSERT(fade_sp.getEnd()==205);

  // ================================================================
  // NB: when source has definite extent, fade in starts at
  // source->getStart() and ends at source->getEnd().
  fprintf(stderr, "=== cropped source, start=TickUtils::indefinite(), end=TickUtils::indefinite()\n");
  fade_sp.setSource(&crop_sp);
  fade_sp.setStart(mu::TickUtils::indefinite()).setEnd(mu::TickUtils::indefinite()).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==100);
  ASSERT(fade_sp.getEnd()==200);

  // ================================================================
  fprintf(stderr, "=== cropped source, start=100, end=200\n");
  fade_sp.setSource(&crop_sp);
  fade_sp.setStart(100).setEnd(200).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==100);
  ASSERT(fade_sp.getEnd()==200);

  // ================================================================
  // When both source stream and fade stream have definite extent, the
  // start time is 
  // max(source->getStart(),fader->getStart()+(fader->getFadeTime()/2))
  // and end time is
  // min(source->getEnd(),fader->getEnd()-(fader->getFadeTime()/2))
  //
  fprintf(stderr, "=== cropped source, start=105, end=195\n");
  fade_sp.setSource(&crop_sp);
  fade_sp.setStart(105).setEnd(195).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==100);
  ASSERT(fade_sp.getEnd()==200);

  // ================================================================
  // When both source stream and fade stream have definite extent, the
  // start time is 
  // max(source->getStart(),fader->getStart()+(fader->getFadeTime()/2))
  // and end time is
  // min(source->getEnd(),fader->getEnd()-(fader->getFadeTime()/2))
  //
  fprintf(stderr, "=== cropped source, start=110, end=190\n");
  fade_sp.setSource(&crop_sp);
  fade_sp.setStart(110).setEnd(190).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==105);
  ASSERT(fade_sp.getEnd()==195);

  // ================================================================
  // When start and end times are close together, it fades in and fades
  // out without reaching unity.
  fprintf(stderr, "=== cropped source, start=150, end=150\n");
  fade_sp.setSource(&crop_sp);
  fade_sp.setStart(150).setEnd(150).setFadeTime(10);

  fade_sp.step(buffer, 0, true);

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

  ASSERT(fade_sp.getStart()==145);
  ASSERT(fade_sp.getEnd()==155);

  // ================================================================
  // coverage tests

  fprintf(stderr, "=== Coverage A\n");
  fade_sp.setSource(NULL).setStart(mu::TickUtils::indefinite()).setEnd(mu::TickUtils::indefinite()).setFadeTime(100);
  fade_sp.step(buffer, 0, true);

  ASSERT(near(buffer(0,0), 0.0));
  ASSERT(near(buffer(0,1), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 0.0));

  fprintf(stderr, "=== Coverage B\n");
  // intentionally set end before start
  fade_sp.setSource(&constant_sp).setStart(200).setEnd(100).setFadeTime(100);
  fade_sp.step(buffer, 0, true);

  ASSERT(near(buffer(0,0), 0.0));
  ASSERT(near(buffer(0,1), 0.0));
  ASSERT(near(buffer(100,0), 0.0));
  ASSERT(near(buffer(100,1), 0.0));
  ASSERT(near(buffer(150,0), 0.0));
  ASSERT(near(buffer(150,1), 0.0));
  ASSERT(near(buffer(200,0), 0.0));
  ASSERT(near(buffer(200,1), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 0.0));

  fprintf(stderr, "=== Coverage C\n");
  fade_sp.setSource(&constant_sp).setStart(150).setEnd(150).setFadeTime(10);
  fade_sp.step(buffer, 0, true);

  ASSERT(near(buffer(0,0), 0.0));
  ASSERT(near(buffer(0,1), 0.0));
  ASSERT(near(buffer(145,0), 0.0));
  ASSERT(near(buffer(145,1), 0.0));
  ASSERT(near(buffer(150,0), 0.5));
  ASSERT(near(buffer(150,1), 0.5));
  ASSERT(near(buffer(155,0), 0.0));
  ASSERT(near(buffer(155,1), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 0.0));

  fprintf(stderr, "=== Coverage D, E, G, H\n");
  fade_sp.setSource(&constant_sp).setStart(120).setEnd(180).setFadeTime(10);
  fade_sp.step(buffer, 0, true);

  ASSERT(near(buffer(0,0), 0.0));
  ASSERT(near(buffer(0,1), 0.0));
  ASSERT(near(buffer(115,0), 0.0));
  ASSERT(near(buffer(115,1), 0.0));
  ASSERT(near(buffer(125,0), 1.0));
  ASSERT(near(buffer(125,1), 1.0));
  ASSERT(near(buffer(175,0), 1.0));
  ASSERT(near(buffer(175,1), 1.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 0.0));

  fprintf(stderr, "=== Coverage F\n");
  fade_sp.setSource(&constant_sp).setStart(0).setEnd(2000).setFadeTime(10);
  fade_sp.step(buffer, 512, true);

  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(near(buffer(0,1), 1.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 1.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 1.0));

  fprintf(stderr, "=== Coverage I\n");
  fade_sp.setSource(&constant_sp).setStart(0).setEnd(512).setFadeTime(10);
  fade_sp.step(buffer, 517, true);

  ASSERT(near(buffer(0,0), 0.0));
  ASSERT(near(buffer(0,1), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 0), 0.0));
  ASSERT(near(buffer(buffer.frames()-1, 1), 0.0));

  // ================================================================
  fprintf(stderr,"=== done\n");
  return 0;

}
