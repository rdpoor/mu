/*
 * Test LoopStream using IdentityStream and MapStream
 */
#include "file_read_stream.h"
#include "loop_stream.h"
#include "add_stream.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define LOOP_INTERVAL_A (10843)
#define LOOP_INTERVAL_B (10712)
#define FILE "/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav"

int main() {
  mu::FileReadStream file_read_stream_a;
  mu::FileReadStream file_read_stream_b;
  mu::LoopStream loop_stream_a;
  mu::LoopStream loop_stream_b;
  mu::AddStream add_stream;
  mu::RtPlayer player;

  file_read_stream_a.fileName(FILE).doNormalize(true);
  file_read_stream_b.fileName(FILE).doNormalize(true);

  mu::Stream *a = &loop_stream_a.setSource(&file_read_stream_a).setLoopDuration(LOOP_INTERVAL_A);
  mu::Stream *b = &loop_stream_b.setSource(&file_read_stream_b).setLoopDuration(LOOP_INTERVAL_B);

  add_stream.addSource(a);
  add_stream.addSource(b);

  player.setSource(&add_stream);

  player.start();
  sleep(300);
  player.stop();

  return 0;
}
