/*
 * SequenceStream accepts an arbitrary number of source streams,
 * assigns a time offset to each stream, and sums the results
 * together.  At present, SequenceStream does not crop the input
 * streams, so overlap is permitted.
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

#ifndef MU_SEQUENCE_STREAM_H
#define MU_SEQUENCE_STREAM_H

#include "mu.h"
#include "add_stream.h"

namespace mu {

  class SequenceStream : public AddStream {
  public:

    SequenceStream( void );
    ~SequenceStream( void );

    std::string getClassName() { return "SequenceStream"; }

    SequenceStream& addSource(Stream *source, Tick delay);

  };                            // class SequenceStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
