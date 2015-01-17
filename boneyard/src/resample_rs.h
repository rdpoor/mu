/*
 * ResampleRS has two inputs: a sample stream and a timing stream.
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

#ifndef MU_RESAMPLE_RS_H
#define MU_RESAMPLE_RS_H

#include "mu_types.h"
#include "render_stream.h"

namespace mu {

  class ResampleRS : public RenderStream {
  public:

    ResampleRS( void );
    ~ResampleRS( void );

    bool render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);

    RenderStream *sample_source( void ) { return sample_source_; }
    void set_sample_source(RenderStream *source) { sample_source_ = source; }

    RenderStream *timing_source( void ) { return timing_source_; }
    void set_timing_source(RenderStream *source) { timing_source_ = source; }

  protected:
    RenderStream *sample_source_;
    RenderStream *timing_source_;
    stk::StkFrames sample_buffer_;
    stk::StkFrames timing_buffer_;

  };                            // class ResampleRS
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
