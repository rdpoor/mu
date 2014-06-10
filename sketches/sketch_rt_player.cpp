/*
 * Test RtPlayer for memory leaks.  Simply starts a stream, and
 * assumes that (in another terminal), you will run ps -v <proc_id>
 */

#include "rt_player.h"

int main() {
  mu::RtPlayer player;
  player.start();
  fprintf(stderr, "Put this job in the background and run ps -v to watch its memory size.\n");
  while (1) { sleep(1000); }
  // should never get here...
  player.stop();

  return 0;
}
  
