/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  

   ================================================================ 
*/

// File: pluck_inst.h
// Defines a plucked instrument with pluck, hammer and dampen methods.  The
// "note" is a stream, but usually a plucked string sound file.  Note that a
// final dampen() must be called to flush the final note.

#ifndef MU_PLUCK_INST_H
#define MU_PLUCK_INST_H

#include "crop_stream.h"
#include "mu_inst.h"
#include "sequence_stream.h"

namespace mu {

  class PluckInst : public MuInst {
  public:

    PluckInst();

    virtual ~PluckInst();

    // start playing a note
    void pluck(MuTick start, MuStream *source, MuFloat gain_db = 0.0);

    // truncate the previous note, start the new note 'd' seconds after its
    // start, where 'd' is the time elapsed between previous note and this.
    void hammer(MuTick start, MuStream *source, MuFloat gain_db = 0.0);

    // stop playing the current note.  In actuality, this emits the previous
    // note, and you must call dampen() to cause the final note to be emitted.
    void dampen(MuTick start);

    // return the stream with the collection of plucked, hammered and dampened
    // notes.
    MuStream *stream() { return sequence_stream_; }

  protected:
    SequenceStream *sequence_stream_;
    MuStream *prev_note_;
    MuTick prev_start_;
    MuFloat prev_gain_db_;
    void emit_prev_note(mu::MuTick start);
  };

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
