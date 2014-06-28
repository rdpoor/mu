/*
 * Test PsiSP.  This particular one: Plays the note shifted down 8
 * semitones Leaves the attack at the original rate Slows the rate
 * down to "zero" (sustained note) Adds vibrato partway through the
 * note. Extends the overall duration about 10x.
 */

#include "add_sp.h"
#include "constant_sp.h"
#include "crop_sp.h"
#include "file_read_sp.h"
#include "file_write_sp.h"
#include "linseg_sp.h"
#include "mu.h"
#include "multiply_sp.h"
#include "rt_player.h"
#include "sine_sp.h"
#include "psi_sp.h"
#include <unistd.h>

#define PSI_FILE "/Users/r/Projects/Mu/SoundSets/A/69.psi"

int main() {

  mu::AddSP pitch_sp;
  mu::ConstantSP center_frequency_sp;
  mu::CropSP crop_sp;
  mu::FileWriteSP file_write_sp;
  mu::LinsegSP time_sp;
  mu::LinsegSP theta_sp;
  mu::LinsegSP vibrato_envelope;
  mu::MultiplySP vibrato_sp;
  mu::RtPlayer player;
  mu::SineSP sine_sp;
  mu::PsiSP psi_sp;

  // ================================================================
  // timing
  // 
  double ddur = 10.0;            // time warp factor after initial attack
  double dpit = pow(2.0, -3/12.0); // pitch shift factor

  psi_sp.setPsiFileName(PSI_FILE);
  mu::Tick frame_count = psi_sp.getFrameCount();

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
  // pitch_sp: constant dpit + enveloped vibrato

  center_frequency_sp.setValue(dpit);

  sine_sp.setFrequency(4.0).setAmplitude(1.0);
  vibrato_sp.addSource(&sine_sp);
  vibrato_sp.addSource(&vibrato_envelope);

  vibrato_envelope.addBreakpoint(t0, 0.0);
  vibrato_envelope.addBreakpoint(t1, 0.0);
  vibrato_envelope.addBreakpoint(t2, 0.02);
  vibrato_envelope.addBreakpoint(t3, 0.0);
  
  pitch_sp.addSource(&center_frequency_sp);
  pitch_sp.addSource(&vibrato_sp);

  // ================================================================
  // time_sp: dtau/dt = 1.0 during initial transient, then ddur

  time_sp.addBreakpoint(t0, u0);
  time_sp.addBreakpoint(t1, u1);
  time_sp.addBreakpoint(t3, u3);

  // ================================================================
  // set up stream network

  psi_sp.setOmegaSource(&pitch_sp);
  psi_sp.setTauSource(&time_sp);

  crop_sp.setSource(&psi_sp).setEnd(t3);
  file_write_sp.setSource(&crop_sp).setFileName("/tmp/psi_69.wav");
  player.setSource(&file_write_sp);

  // ================================================================
  // Go!

  player.start();
  fprintf(stderr, "Type [return] to quit:"); getchar();
  player.stop();
}


  

