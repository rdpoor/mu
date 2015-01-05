/*
 * Unit Test FileWriteSP
 */
#include "assert.h"
#include "crop_sp.h"
#include "file_write_sp.h"
#include "mu.h"
#include "nrt_player.h"
#include "sine_sp.h"
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
  mu::CropSP crop_sp;
  mu::FileWriteSP file_write_sp;
  mu::NrtPlayer player;
  mu::SineSP sine_sp;
  
  sine_sp.setFrequency(440.0).setAmplitude(mu::dbToValue(-10)).setPhase(0.0);
  crop_sp.setSource(&sine_sp).setStart(0).setEnd(44100);
  file_write_sp.setSource(&crop_sp).setFileName(TEST_FILE_NAME);
  player.setSource(&file_write_sp);

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
