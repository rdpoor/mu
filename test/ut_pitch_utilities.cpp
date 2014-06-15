// Test PitchUtilities class

#include "pitch_utilities.h"
#include "assert.h"
#include "mu.h"

int main() {
  mu::PitchUtilities pu;

  ASSERT(near(pu.midiPitchToFrequency(0), 8.175798916));
  ASSERT(near(pu.midiPitchToFrequency(69), 440.0));
  ASSERT(near(pu.midiPitchToFrequency(127), 12543.85395));

  ASSERT(near(pu.midiPitchToPitchClass(0), 0));
  ASSERT(near(pu.midiPitchToPitchClass(24), 0));
  ASSERT(near(pu.midiPitchToPitchClass(24.5), 0.5));
  ASSERT(near(pu.midiPitchToPitchClass(-1.0), 11.0));
  ASSERT(near(pu.midiPitchToPitchClass(-0.5), 11.5));
  
  ASSERT(near(pu.pitchClassToMidiPitch(0, -1), 0));
  ASSERT(near(pu.pitchClassToMidiPitch(0, 0), 12));
  ASSERT(near(pu.pitchClassToMidiPitch(0, 1), 24));
  ASSERT(near(pu.pitchClassToMidiPitch(1.5, 1), 25.5));

  ASSERT(pu.pitchNameToPitchClass("c") == 0);
  ASSERT(pu.pitchNameToPitchClass("cs") == 1);
  ASSERT(pu.pitchNameToPitchClass("df") == 1);
  ASSERT(pu.pitchNameToPitchClass("c4") == 0);
  ASSERT(pu.pitchNameToPitchClass("cs4") == 1);
  ASSERT(pu.pitchNameToPitchClass("df4") == 1);
  ASSERT(pu.pitchNameToPitchClass("c-1") == 0);
  ASSERT(pu.pitchNameToPitchClass("cs-1") == 1);
  ASSERT(pu.pitchNameToPitchClass("df-1") == 1);
  
  ASSERT(pu.pitchNameToOctave("c") == 0);
  ASSERT(pu.pitchNameToOctave("c-1") == -1);
  ASSERT(pu.pitchNameToOctave("c10") == 10);
  
  ASSERT(near(pu.pitchNameToFrequency("c"), 16.35159783));
  ASSERT(near(pu.pitchNameToFrequency("a"), 27.5));

  ASSERT(near(pu.pitchNameToFrequency("c-1"), 8.175798916));
  ASSERT(near(pu.pitchNameToFrequency("cs-1"), 8.661957218));
  ASSERT(near(pu.pitchNameToFrequency("df-1"), 8.661957218));
  ASSERT(near(pu.pitchNameToFrequency("a-1"), 13.75));

  ASSERT(near(pu.pitchNameToFrequency("c9"),8372.01809));
  ASSERT(near(pu.pitchNameToFrequency("cs9"), 8869.844191));
  ASSERT(near(pu.pitchNameToFrequency("df9"), 8869.844191));
  ASSERT(near(pu.pitchNameToFrequency("a9"), 14080.0));

  return 0;
}
