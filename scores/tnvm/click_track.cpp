/*
 * Together Now, Very Minor.  Click track.
 */
#include "bass.h"
#include "layout.h"

#include "file_read_stream.h"
#include "loop_stream.h"
#include "multiply_stream.h"
#include "constant_stream.h"
#include "add_stream.h"

#define PERCUSSION_SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/ThumpsAndScratches/"

mu::Stream *make_click_track_stream() {
  mu::FileReadStream *thump_stream = new mu::FileReadStream();
  thump_stream->fileName(PERCUSSION_SOUND_DIR "s15.wav");

  mu::FileReadStream *tic_stream = new mu::FileReadStream();
  tic_stream->fileName(PERCUSSION_SOUND_DIR "s11.wav");

  mu::ConstantStream *tic_gain = new mu::ConstantStream();
  tic_gain->setValue(0.25);

  mu::MultiplyStream *tic_attenuator = new mu::MultiplyStream();
  tic_attenuator->addSource(tic_gain);
  tic_attenuator->addSource(tic_stream);

  mu::LoopStream *tic_loop = new mu::LoopStream();
  tic_loop->setSource(tic_attenuator);
  tic_loop->setLoopDuration(BEAT_TO_FRAME(1));
  tic_loop->setStart(0);
  tic_loop->setEnd(BEAT_TO_FRAME(4));

  mu::AddStream *mix_stream = new mu::AddStream();
  mix_stream->addSource(thump_stream);
  mix_stream->addSource(tic_loop);

  mu::LoopStream *loop_stream = new mu::LoopStream();
  loop_stream->setLoopDuration(MEASURE_TO_FRAME(1));
  loop_stream->setSource(mix_stream);
  loop_stream->setStart(MEASURE_TO_FRAME(M_INTRO));
  loop_stream->setEnd(MEASURE_TO_FRAME(M_END));

  return loop_stream;
}


