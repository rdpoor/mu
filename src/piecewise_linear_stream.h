/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ================================================================
*/

/*
 * PiecewiseLinearStream(t) provides linearly interpolated values between
 * user-provided breakpoints.  If t is less than the time of the first
 * breakpoint, the value of the first breakpoint is returned.  If t is equal to
 * or greater than the time of the last breakpoint, the value of the last
 * breakpoint is returned.
 */

#ifndef MU_PIECEWISE_LINEAR_STREAM_H
#define MU_PIECEWISE_LINEAR_STREAM_H

#include "mu_stream.h"
#include "mu_types.h"

namespace mu {

  class PiecewiseLinearStream : public MuStream {
  public:

    PiecewiseLinearStream( void );
    virtual ~PiecewiseLinearStream( void );
    virtual PiecewiseLinearStream *clone( void );

    void add_breakpoint(MuTick tick, MuFloat value);

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "PiecewiseLinearStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);
    MuBreakpoints breakpoints_;
    
  private:
    MuFloat lerp(MuTick frame);

  };                            // class PiecewiseLinearStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
