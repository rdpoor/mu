/*
 * More strumming.  Work out compact syntax.
 * See also mune12.cpp and mune17.cpp
 */
#include "add_stream.h"
#include "crop_stream.h"
#include "delay_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"
#include "sequence_stream.h"
#include <unistd.h>

#define SOUND_DIR "/Users/r/Projects/Musics/TNVM/sources/PluckFinger/"

#define CREATE_VECTOR(_TYPE_, _ARRAY_) std::vector<_TYPE_>(_ARRAY_, (_ARRAY_ + sizeof(_ARRAY_) / SIZEOF(_TYPE_)))

/* 
 * I like the idea of defining a fretted instrument that, given fret
 * positions, creates a stream object to strum that chord.
 */

class FrettedInstrument {
public:
  // set the number and tuning of strings for this instrument
  FrettedInstrument& defineStrings(std::vector<int>pitches);

  /*
   * Generate a stream object that produces a single strum at time 0
   * with fingers on the given frets.  A fret value of 0 means open
   * string, a negative fret value means the string is damped.
   * Direction (kUp or kDown) specifies the order of strum.  If given,
   * dynamics_lo specifies the loudness of the strum on the
   * bottom-most string in relative dB, dynamics_hi specifies the
   * loudness of the strum on the topmost string.  The loudness for
   * the intermediate strings are interpolated.
   *
   * TODO: can we define a pluckable string that rings until it is
   * plucked again at some (indeterminate) later time?  (Answer:
   * yes, see SpliceStream below...)
   */
  mu::Stream *strum(std::vector<int>frets, bool direction, double dynamics_lo = 0.0, double dynamics_hi = 0.0);

protected:
  
}; 

