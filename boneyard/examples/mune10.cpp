/*
 * Test LoopSP using IdentitySP and MapSP
 */
#include "file_read_sp.h"
#include "loop_sp.h"
#include "add_sp.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define LOOP_INTERVAL_A (10843)
#define LOOP_INTERVAL_B (10712)
#define FILE "/Users/r/Projects/Musics/TNVM/sources/Chords/A.wav"

int main() {
  mu::FileReadSP file_read_sp;
  mu::LoopSP loop_sp_a;
  mu::LoopSP loop_sp_b;
  mu::AddSP add_sp;
  mu::RtPlayer player;

  file_read_sp.fileName(FILE).doNormalize(true);
  loop_sp_a.setSource(&file_read_sp).setLoopDuration(LOOP_INTERVAL_A);
  loop_sp_b.setSource(&file_read_sp).setLoopDuration(LOOP_INTERVAL_B);
  add_sp.addSource(&loop_sp_a);
  add_sp.addSource(&loop_sp_b);

  player.setSource(&add_sp);

  player.start();
  sleep(60);
  player.stop();

  return 0;
}
