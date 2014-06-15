/*
 * Test a slide guitar-ish thing using PsiStream.
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

#define PSI_FILE "/Users/r/Projects/Mu/SoundSets/A/69.psi"

class PitchMap {
  PitchMap(void) {

  }
  double pitchToFrequency(std::string pitch) {
    return map_[pitch];
  }
protected:
  std::map<std::string, double> map_;
}

class StringNote {
public:
protected:
  mu::LinSegStream time_stream_;
  mu::LinSegStream pitch_stream_;
  mu::PsiStream psi_stream_;
  double original_period_;
};                              // class StringNote

class String {
public:
  // pluck() emits any previous note and starts a new one.
  // gliss() extends the current note.
  // rest() emits any previous note.
protected:
  
};                              // class String

Stream& generateNotes(Hand& hand) {
  hand.pluck(dur_q, "e4", 0.0);
  hand.gliss(dur_e, "c5", gliss_rate);
  hand.gliss(dur_q, "b4", gliss_rate);
  hand.gliss(dur_q, "g4", gliss_rate);
  hand.gliss(dur_q, "e4", gliss_rate);
  hand.pluck(dur_q, "e4", gliss_rate);
  hand.gliss(dur_e, "d4", gliss_rate);
  hand.gliss(dur_e, "g4", gliss_rate);
  hand.gliss(dur_q, "e4", gliss_rate);
}
