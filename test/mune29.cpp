/*
 * Test SplayStream.  This particular one:
 * Plays the note shifted down 8 semitones
 * Leaves the attack at the original rate
 * Slows the rate down to "zero" (sustained note)
 * Adds vibrato partway through the note
 * Extends the overall duration about 10x.
 */

#include "add_stream.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "linseg_stream.h"
#include "mu.h"
#include "multiply_stream.h"
#include "rt_player.h"
#include "sine_stream.h"
#include "splay_stream.h"
#include <unistd.h>

int main() {

  mu::AddStream add_stream;
  mu::CropStream crop_stream;
  mu::FileReadStream file_read_stream;
  mu::FileWriteStream file_write_stream;
  mu::LinsegStream phi_stream;
  mu::LinsegStream theta_stream;
  mu::LinsegStream vibrato_envelope;
  mu::MultiplyStream vibrato_stream;
  mu::RtPlayer player;
  mu::SineStream sine_stream;
  mu::SplayStream splay_stream;

#define SOUND_FILE "/Users/r/Projects/Mu/SoundSets/A/69.wav"
#define PERIOD 104.7
#define ATTACK_TIME 1800          // first N frames are part of the attack

  double ddur = 10.0;
  double dpit = pow(2.0, -8/12.0);

  file_read_stream.fileName(SOUND_FILE).doNormalize(true);
  mu::Tick original_frame_count = file_read_stream.getEnd();
  mu::Tick original_period_count = original_frame_count / PERIOD;

  fprintf(stderr, "original_frame_count = %ld, original_period_count = %ld\n",
          original_frame_count, original_period_count);

  sine_stream.setFrequency(4.0).setAmplitude(1.0);
  vibrato_stream.addSource(&sine_stream);
  vibrato_stream.addSource(&vibrato_envelope);

  vibrato_envelope.addBreakpoint(0, 0.0);
  vibrato_envelope.addBreakpoint(100000, 0.0);
  vibrato_envelope.addBreakpoint(200000, 2.0);
  vibrato_envelope.addBreakpoint(300000, 0.0);
  
  add_stream.addSource(&vibrato_stream);
  add_stream.addSource(&theta_stream);

  splay_stream.setSampleSource(&file_read_stream).
    setPeriod(PERIOD).
    setNPeriods(original_period_count);
  splay_stream.setThetaSource(&add_stream);
  splay_stream.setPhiSource(&phi_stream);
  file_write_stream.setSource(&splay_stream).setFileName("/tmp/69.wav");
  player.setSource(&file_write_stream);

  phi_stream.addBreakpoint(0,0);  
  phi_stream.addBreakpoint(ATTACK_TIME, ATTACK_TIME / PERIOD); // leave attack unchanged
  phi_stream.addBreakpoint(200000, ATTACK_TIME / PERIOD);      // time stands still
  // phi_stream.addBreakpoint(300000, 0);                         // goes backwards
  phi_stream.addBreakpoint(original_frame_count * ddur,        // plays out to end
                           original_period_count);

  // slope = 2pi * dpit / period
  theta_stream.addBreakpoint(0, 0);
  theta_stream.addBreakpoint(original_frame_count * ddur, 
                             original_frame_count * ddur * 2.0 * M_PI * dpit / PERIOD);

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();
}


  

