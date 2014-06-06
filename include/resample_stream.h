/*
 * ResampleStream has two inputs: a sample stream and a timing stream.
 * When called to produce samples, it uses the timing stream to create
 * a timing map into the sample data.  It then fetches the sample data
 * and uses linear interpolation to generate output samples.
 */
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

#ifndef MU_RESAMPLE_STREAM_H
#define MU_RESAMPLE_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class ResampleStream : public Stream {
  public:

    ResampleStream( void );
    ~ResampleStream( void );

    std::string getClassName() { return "ResampleStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    ResampleStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Stream *getSampleSource( void ) { return sample_source_; }
    ResampleStream& setSampleSource(Stream *source) { sample_source_ = source; return *this; }

    Stream *getTimingSource( void ) { return timing_source_; }
    ResampleStream& setTimingSource(Stream *source) { timing_source_ = source; return *this; }

    // getStart() and getEnd() can only be properly computed by taking
    // the min and max of the entire timingStream.  Better just to say 
    // kIndefinite.

  protected:
    Stream *sample_source_;
    Stream *timing_source_;
    stk::StkFrames sample_buffer_;
    stk::StkFrames timing_buffer_;

  };                            // class ResampleStream
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
