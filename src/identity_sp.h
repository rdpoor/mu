/*
 * IdentitySP is a source of test data.  It simply outputs
 * a value of F at frame F.
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

#ifndef MU_IDENTITY_STREAM_H
#define MU_IDENTITY_STREAM_H

#include "mu.h"
#include "sample_processor.h"

namespace mu {

  class IdentitySP : public SampleProcessor {
  public:

    IdentitySP( void );
    ~IdentitySP( void );

    std::string getClassName() { return "IdentitySP"; }
    void step(stk::StkFrames& buffer, Tick tick, bool is_new_event);

  };                            // class IdentitySP

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
