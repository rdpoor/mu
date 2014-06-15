/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/

#ifndef MU_PITCH_UTILITIES_H
#define MU_PITCH_UTILITIES_H

#include <string>
#include <map>
#include <math.h>
#include <regex>

namespace mu {
  class PitchUtilities {

  public:

    PitchUtilities( void ) {
      pitch_class_map_["c"] = 0;
      pitch_class_map_["cs"] = 1;
      pitch_class_map_["df"] = 1;
      pitch_class_map_["d"] = 2;
      pitch_class_map_["ds"] = 3;
      pitch_class_map_["ef"] = 3;
      pitch_class_map_["e"] = 4;
      pitch_class_map_["f"] = 5;
      pitch_class_map_["fs"] = 6;
      pitch_class_map_["gf"] = 6;
      pitch_class_map_["g"] = 7;
      pitch_class_map_["gs"] = 8;
      pitch_class_map_["af"] = 8;
      pitch_class_map_["a"] = 9;
      pitch_class_map_["as"] = 10;
      pitch_class_map_["bf"] = 10;
      pitch_class_map_["b"] = 11;
    }

    double midiPitchToPitchClass( double midi_pitch ) {
      double pc = fmod(midi_pitch, 12.0);
      return (pc < 0.0) ? pc + 12.0 : pc;
    }

    double midiPitchToOctave( double midi_pitch ) {
      return floor(midi_pitch/12.0) - 1;
    }

    double midiPitchToFrequency( double midi_pitch ) {
      return pow(2.0, (midi_pitch - 69.0) / 12.0)  * 440.0;
    }

    double pitchClassToMidiPitch( double pitch_class, int octave = 0) {
      return pitch_class + (12 * (octave + 1));
    }

    int pitchNameToMidiPitch( std::string pitch_name ) {
      std::smatch sm;
      std::regex e ("\([abcdefg][fs]?)(-?[0123456789]*)");
      if (regex_match(pitch_name, sm, e)) {
        int pc = pitch_class_map_[sm[1]];
        int oct = (sm[2] == "") ? 0 : std::stoi(sm[2]);
        return pitchClassToMidiPitch(pc, oct);
      } else {
        return -1;              // TODO: could be better -- raise error?
      }
    }

    int pitchNameToPitchClass( std::string pitch_name ) {
      return midiPitchToPitchClass(pitchNameToMidiPitch(pitch_name));
    }

    int pitchNameToOctave( std::string pitch_name ) {
      return midiPitchToOctave(pitchNameToMidiPitch(pitch_name));
    }

    double pitchNameToFrequency( std::string pitch_name ) {
      return midiPitchToFrequency(pitchNameToMidiPitch(pitch_name));
    }

  protected:
    std::map<std::string, int> pitch_class_map_;
  };
}

#endif

// Local Variables:
// mode: c++
// End:
