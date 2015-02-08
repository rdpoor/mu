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
 * StepStream(t) provides linearly interpolated values between
 * user-provided breakpoints.  If t is less than the time of the first
 * breakpoint, the value of the first breakpoint is returned.  If t is equal to
 * or greater than the time of the last breakpoint, the value of the last
 * breakpoint is returned.
 */

#ifndef MU_STEP_STREAM_H
#define MU_STEP_STREAM_H

#include "mu_stream.h"
#include "mu_types.h"

namespace mu {

  class StepStream : public MuStream {
  public:

    StepStream( void );
    virtual ~StepStream( void );
    virtual StepStream *clone( void );

    void add_breakpoint(MuTick tick, MuFloat value);

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "StepStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);
    MuBreakpoints breakpoints_;
    
  private:
    // TODO: can I put this in the .cpp file and make it inline?
    // It would be nice to keep the .h file visually simple.
    MuFloat compute_sample(MuTick frame) {
      if (frame <= breakpoints_.begin()->first) {
        return breakpoints_.begin()->second;
      }

      if (frame >= breakpoints_.rbegin()->first) {
        return breakpoints_.rbegin()->second;
      } 

      // frame lies within two breakpoints
      MuBreakpoints::const_iterator cursor = breakpoints_.upper_bound(frame);
      --cursor;
      MuFloat y0 = cursor->second;
      return y0;
    }

  };                            // class StepStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
