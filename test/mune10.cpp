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
  mu::FileReadStream file_read_stream;
  mu::LoopStream loop_stream_a;
  mu::LoopStream loop_stream_b;
  mu::AddStream add_stream;
  mu::RtPlayer player;

  file_read_stream.fileName(FILE).doNormalize(true);
  loop_stream_a.setSource(&file_read_stream).setLoopDuration(LOOP_INTERVAL_A);
  loop_stream_b.setSource(&file_read_stream).setLoopDuration(LOOP_INTERVAL_B);
  add_stream.addSource(&loop_stream_a);
  add_stream.addSource(&loop_stream_b);

  player.setSource(&add_stream);

  player.start();
  sleep(60);
  player.stop();

  return 0;
}
