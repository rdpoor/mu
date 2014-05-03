/*
 * Make an arpeggiated chord.  Is there a way to simplify this?
 */
#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
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
  mu::FileReadStream file_read_stream_0;
  mu::FileReadStream file_read_stream_1;
  mu::FileReadStream file_read_stream_2;
  mu::FileReadStream file_read_stream_3;
  mu::CropStream crop_stream_0;
  mu::CropStream crop_stream_1;
  mu::CropStream crop_stream_2;
  mu::CropStream crop_stream_3;
  mu::DelayStream delay_stream_0;
  mu::DelayStream delay_stream_1;
  mu::DelayStream delay_stream_2;
  mu::DelayStream delay_stream_3;
  mu::AddStream add_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  // connect elements

  fprintf(stderr, "A");
  crop_stream_0.setSource(&file_read_stream_0);
  crop_stream_1.setSource(&file_read_stream_1);
  crop_stream_2.setSource(&file_read_stream_2);
  crop_stream_3.setSource(&file_read_stream_3);

  fprintf(stderr, "B");
  delay_stream_0.setSource(&crop_stream_0);
  delay_stream_1.setSource(&crop_stream_1);
  delay_stream_2.setSource(&crop_stream_2);
  delay_stream_3.setSource(&crop_stream_3);

  fprintf(stderr, "C");
  add_stream.addSource(&delay_stream_0);
  add_stream.addSource(&delay_stream_1);
  add_stream.addSource(&delay_stream_2);
  add_stream.addSource(&delay_stream_3);

  fprintf(stderr, "D");
  loop_stream.setSource(&add_stream);

  fprintf(stderr, "E");
  player.setSource(&loop_stream);

  fprintf(stderr, "F");

  // configure elements

  file_read_stream_0.fileName(SOUND_DIR SOUND_0 ".wav").doNormalize(true);
  file_read_stream_1.fileName(SOUND_DIR SOUND_1 ".wav").doNormalize(true);
  file_read_stream_2.fileName(SOUND_DIR SOUND_2 ".wav").doNormalize(true);
  file_read_stream_3.fileName(SOUND_DIR SOUND_3 ".wav").doNormalize(true);

  fprintf(stderr, "file_read_stream_0.getEnd() = %ld\n", file_read_stream_0.getEnd());
  fprintf(stderr, "file_read_stream_1.getEnd() = %ld\n", file_read_stream_1.getEnd());
  fprintf(stderr, "file_read_stream_2.getEnd() = %ld\n", file_read_stream_2.getEnd());
  fprintf(stderr, "file_read_stream_3.getEnd() = %ld\n", file_read_stream_3.getEnd());

  crop_stream_0.setStart(0);    // because FileReadStream can't tolerate negative tick times
  crop_stream_1.setStart(0);
  crop_stream_2.setStart(0);
  crop_stream_3.setStart(0);

  fprintf(stderr, "crop_stream_0.getEnd() = %ld\n", crop_stream_0.getEnd());
  fprintf(stderr, "crop_stream_1.getEnd() = %ld\n", crop_stream_1.getEnd());
  fprintf(stderr, "crop_stream_2.getEnd() = %ld\n", crop_stream_2.getEnd());
  fprintf(stderr, "crop_stream_3.getEnd() = %ld\n", crop_stream_3.getEnd());

  delay_stream_0.setDelay(882 * 1); // make this relative to player.getFrameRate()
  delay_stream_1.setDelay(882 * 2);
  delay_stream_2.setDelay(882 * 3);
  delay_stream_3.setDelay(882 * 4);

  fprintf(stderr, "delay_stream_0.getEnd() = %ld\n", delay_stream_0.getEnd());
  fprintf(stderr, "delay_stream_1.getEnd() = %ld\n", delay_stream_1.getEnd());
  fprintf(stderr, "delay_stream_2.getEnd() = %ld\n", delay_stream_2.getEnd());
  fprintf(stderr, "delay_stream_3.getEnd() = %ld\n", delay_stream_3.getEnd());

  fprintf(stderr, "dly = %ld\n", delay_stream_1.getDelay());

  long int dur = std::max(delay_stream_0.getEnd(), 0l);
  dur = std::max(delay_stream_1.getEnd(), dur);
  dur = std::max(delay_stream_2.getEnd(), dur);
  dur = std::max(delay_stream_3.getEnd(), dur);

  fprintf(stderr, "dur = %ld\n", dur);

  loop_stream.setLoopDuration(dur);

  // connect them

  player.start();
  fprintf(stderr, "G");
  sleep(300);
  player.stop();

  return 0;
}
