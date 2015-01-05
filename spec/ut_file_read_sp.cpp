/*
 * Unit Test FileReadSP
 */
#include "assert.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "file_read_sp.h"
#include "file_write_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

#define TEST_FILE_NAME "/Users/r/Projects/Mu/SoundSets/tmp/constant.wav"

void createTestFile() {
  mu::CropSP crop_sp;
  mu::ConstantSP constant_sp;
  mu::FileWriteSP file_write_sp;
  mu::NrtPlayer player;

  constant_sp.setValue(1.0);
  crop_sp.setSource(&constant_sp).setStart(0).setEnd(44100);
  file_write_sp.setSource(&crop_sp).setFileName(TEST_FILE_NAME);
  player.setSource(&file_write_sp);

  player.start();
  while (!player.isRunning()) { 
    usleep(1); 
  }
  while (player.isRunning()) { 
    sleep(1); 
  }

}

int main() {
  mu::FileReadSP file_read_sp;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  createTestFile();

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  file_read_sp.fileName(TEST_FILE_NAME).doNormalize(true);

  file_read_sp.step(buffer, 0, true);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(near(buffer((FRAME_COUNT-1),0), 1.0));

  file_read_sp.step(buffer, -100, true);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(near(buffer(100,0), 1.0));
  ASSERT(near(buffer((FRAME_COUNT-1),0), 1.0));

  file_read_sp.step(buffer, 44100-100, true);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(buffer(100,0) == 0.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0.0);

  file_read_sp.step(buffer, 44100, true);
  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0.0);


  return 0;

}
