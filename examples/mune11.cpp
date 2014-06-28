/*
 * Make an arpeggiated chord.  Is there a way to simplify this?
 */
#include "add_sp.h"
#include "delay_sp.h"
#include "file_read_sp.h"
#include "loop_sp.h"
#include "mu.h"
#include "rt_player.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"
#define SOUND_0 "A4"
#define SOUND_1 "C5"
#define SOUND_2 "E5"
#define SOUND_3 "F#5"
#define ARPEGGIO_DELAY_S 0.1

int main() {
  mu::FileReadSP file_read_sp_0;
  mu::FileReadSP file_read_sp_1;
  mu::FileReadSP file_read_sp_2;
  mu::FileReadSP file_read_sp_3;
  mu::DelaySP delay_sp_0;
  mu::DelaySP delay_sp_1;
  mu::DelaySP delay_sp_2;
  mu::DelaySP delay_sp_3;
  mu::AddSP add_sp;
  mu::LoopSP loop_sp;
  mu::RtPlayer player;

  // connect elements

  fprintf(stderr, "B");
  delay_sp_0.setSource(&file_read_sp_0);
  delay_sp_1.setSource(&file_read_sp_1);
  delay_sp_2.setSource(&file_read_sp_2);
  delay_sp_3.setSource(&file_read_sp_3);

  fprintf(stderr, "C");
  add_sp.addSource(&delay_sp_0);
  add_sp.addSource(&delay_sp_1);
  add_sp.addSource(&delay_sp_2);
  add_sp.addSource(&delay_sp_3);

  fprintf(stderr, "D");
  loop_sp.setSource(&add_sp);

  fprintf(stderr, "E");
  player.setSource(&loop_sp);

  fprintf(stderr, "F");

  // configure elements

  file_read_sp_0.fileName(SOUND_DIR SOUND_0 ".wav").doNormalize(true);
  file_read_sp_1.fileName(SOUND_DIR SOUND_1 ".wav").doNormalize(true);
  file_read_sp_2.fileName(SOUND_DIR SOUND_2 ".wav").doNormalize(true);
  file_read_sp_3.fileName(SOUND_DIR SOUND_3 ".wav").doNormalize(true);

  delay_sp_0.setDelay(882 * 1); // make this relative to player.getFrameRate()
  delay_sp_1.setDelay(882 * 2);
  delay_sp_2.setDelay(882 * 3);
  delay_sp_3.setDelay(882 * 4);

  fprintf(stderr, "dly = %ld\n", delay_sp_1.getDelay());

  long int dur = std::max(delay_sp_0.getEnd(), 0l);
  dur = std::max(delay_sp_1.getEnd(), dur);
  dur = std::max(delay_sp_2.getEnd(), dur);
  dur = std::max(delay_sp_3.getEnd(), dur);

  fprintf(stderr, "dur = %ld\n", dur);

  loop_sp.setLoopDuration(dur/2);

  // connect them

  player.start();
  fprintf(stderr, "G");
  sleep(300);
  player.stop();

  return 0;
}
