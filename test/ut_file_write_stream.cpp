/*
 * Unit Test FileWriteStream
 */
#include "assert.h"
#include "crop_stream.h"
#include "file_write_stream.h"
#include "mu.h"
#include "nrt_player.h"
#include "sine_stream.h"
#include <unistd.h>

#define FRAME_RATE 44100
#define FRAME_COUNT 512
#define CHANNEL_COUNT 2
#define DELAY_P_100 (100)
#define DELAY_M_100 (-100)

// see: ~/Projects/Mu/tarballs/stk-4.4.4/include/FileWrite.h

#define TEST_FILE_NAME "/Users/r/Projects/Mu/SoundSets/tmp/069.wav"
#define REFERENCE_FILE_NAME "/Users/r/Projects/Mu/SoundSets/Tones/069.wav"

int main() {
  mu::CropStream crop_stream;
  mu::FileWriteStream file_write_stream;
  mu::NrtPlayer player;
  mu::SineStream sine_stream;
  
  sine_stream.setFrequency(440.0).setAmplitude(mu::dbToValue(-10)).setPhase(0.0);
  crop_stream.setSource(&sine_stream).setStart(0).setEnd(44100);
  file_write_stream.setSource(&crop_stream).setFileName(TEST_FILE_NAME);
  player.setSource(&file_write_stream);

  player.start();
  while (!player.isRunning()) { 
    usleep(1); 
  }
  while (player.isRunning()) { 
    sleep(1); 
  }

  // check to see that file exists and is identical to
  // REFERENCE_FILE_NAME

  return 0;

}
