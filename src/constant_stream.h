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
 * ConstantStream(t) = v
 * Defined over all values of t.
 */

#ifndef MU_CONSTANT_STREAM_H
#define MU_CONSTANT_STREAM_H

#include "mu_stream.h"
#include "mu_types.h"

namespace mu {

  class ConstantStream : public MuStream {
  public:

    ConstantStream( void );
    virtual ~ConstantStream( void );
    virtual ConstantStream *clone( void );

    MuFloat value() { return value_; }
    void set_value(MuFloat value) { value_ = value; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "ConstantStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    MuFloat value_;
  };                            // class ConstantStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
