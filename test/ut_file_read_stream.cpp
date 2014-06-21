/*
 * Unit Test FileReadStream
 */
#include "assert.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2

#define TEST_FILE_NAME "/Users/r/Projects/Mu/SoundSets/tmp/constant.wav"

void createTestFile() {
  mu::CropStream crop_stream;
  mu::ConstantStream constant_stream;
  mu::FileWriteStream file_write_stream;
  mu::NrtPlayer player;

  constant_stream.setValue(1.0);
  crop_stream.setSource(&constant_stream).setStart(0).setEnd(44100);
  file_write_stream.setSource(&crop_stream).setFileName(TEST_FILE_NAME);
  player.setSource(&file_write_stream);

  player.start();
  while (!player.isRunning()) { 
    usleep(1); 
  }
  while (player.isRunning()) { 
    sleep(1); 
  }

}

int main() {
  mu::FileReadStream file_read_stream;
  mu::NrtPlayer player;
  stk::StkFrames buffer;

  createTestFile();

  buffer.setDataRate(FRAME_RATE);
  buffer.resize(FRAME_COUNT, CHANNEL_COUNT);

  file_read_stream.fileName(TEST_FILE_NAME).doNormalize(true);

  file_read_stream.step(buffer, 0, player);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(near(buffer((FRAME_COUNT-1),0), 1.0));

  file_read_stream.step(buffer, -100, player);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(near(buffer(100,0), 1.0));
  ASSERT(near(buffer((FRAME_COUNT-1),0), 1.0));

  file_read_stream.step(buffer, 44100-100, player);
  ASSERT(near(buffer(0,0), 1.0));
  ASSERT(buffer(100,0) == 0.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0.0);

  file_read_stream.step(buffer, 44100, player);
  ASSERT(buffer(0,0) == 0.0);
  ASSERT(buffer((FRAME_COUNT-1),0) == 0.0);


  return 0;

}
