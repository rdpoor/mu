/*
 * Test PsiStream.  This particular one: Plays the note shifted down 8
 * semitones Leaves the attack at the original rate Slows the rate
 * down to "zero" (sustained note) Adds vibrato partway through the
 * note. Extends the overall duration about 10x.
 */

#include "add_stream.h"
#include "constant_stream.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "file_write_stream.h"
#include "linseg_stream.h"
#include "mu.h"
#include "multiply_stream.h"
#include "rt_player.h"
#include "sine_stream.h"
#include "psi_stream.h"
#include <unistd.h>

#define SOUND_FILE "/Users/r/Projects/Mu/SoundSets/A/69.wav"
// #define PERIOD 104.7
#define PERIOD 100            // intentionally mis-estimate period

int main() {

  mu::AddStream pitch_stream;
  mu::ConstantStream center_frequency_stream;
  mu::CropStream crop_stream;
  mu::FileReadStream file_read_stream;
  mu::FileWriteStream file_write_stream;
  mu::LinsegStream time_stream;
  mu::LinsegStream theta_stream;
  mu::LinsegStream vibrato_envelope;
  mu::MultiplyStream vibrato_stream;
  mu::RtPlayer player;
  mu::SineStream sine_stream;
  mu::PsiStream psi_stream;

  // ================================================================
  // timing
  // 
  double ddur = 10.0;            // time warp factor after initial attack
  double dpit = pow(2.0, -3/12.0); // pitch shift factor

  file_read_stream.fileName(SOUND_FILE).doNormalize(true);
  mu::Tick frame_count = file_read_stream.getEnd();

  // "u" is input (original) time
  double u0 = 0;
  double u1 = 2000;             // initial attack (unmodified)
  double u3 = frame_count;      // end of waveform
  double u2 = (u1 + u3)/2.0;    // max vibrato

  // "t" is output (warped) time
  double t0 = u0;
  double t1 = u1;               // initial attack (unmodified)
  double t2 = u1 + (u2-u1)*ddur;
  double t3 = u1 + (u3-u1)*ddur;

  printf("u0 = %f, t0 = %f\n", u0, t0);
  printf("u1 = %f, t1 = %f\n", u1, t1);
  printf("u2 = %f, t2 = %f\n", u2, t2);
  printf("u3 = %f, t3 = %f\n", u3, t3);

  // ================================================================
  // pitch_stream: constant dpit + enveloped vibrato

  center_frequency_stream.setValue(dpit);

  sine_stream.setFrequency(4.0).setAmplitude(1.0);
  vibrato_stream.addSource(&sine_stream);
  vibrato_stream.addSource(&vibrato_envelope);

  vibrato_envelope.addBreakpoint(t0, 0.0);
  vibrato_envelope.addBreakpoint(t1, 0.0);
  vibrato_envelope.addBreakpoint(t2, 0.02);
  vibrato_envelope.addBreakpoint(t3, 0.0);
  
  pitch_stream.addSource(&center_frequency_stream);
  pitch_stream.addSource(&vibrato_stream);

  // ================================================================
  // time_stream: dtau/dt = 1.0 during initial transient, then ddur

  time_stream.addBreakpoint(t0, u0);
  time_stream.addBreakpoint(t1, u1);
  time_stream.addBreakpoint(t3, u3);

  // ================================================================
  // set up stream network

  psi_stream.setSampleSource(&file_read_stream).setEstimatedPeriod(PERIOD);
  psi_stream.setOmegaSource(&pitch_stream);
  psi_stream.setTauSource(&time_stream);
  printf("initializing psi data..."); fflush(stdout);
  psi_stream.setup(player);     // this can take a while
  printf("\n");

  crop_stream.setSource(&psi_stream).setEnd(t3);
  file_write_stream.setSource(&crop_stream).setFileName("/tmp/psi_69.wav");
  player.setSource(&file_write_stream);

  // ================================================================
  // Go!

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();
}


  

