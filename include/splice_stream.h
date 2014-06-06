/*
 * SpliceStream accepts an arbitrary number of source streams, assigns
 * a time offset to each stream, and plays each stream in sequence,
 * terminating the previous stream when the next stream starts.
 * Compare to SequenceStream (which allows overlap).
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

#ifndef MU_SPLICE_STREAM_H
#define MU_SPLICE_STREAM_H

#include "mu.h"
#include "multi_source_stream.h"

namespace mu {

  class SpliceStream : public MultiSourceStream {
  public:
    
    SpliceStream( void );
    ~SpliceStream( void );

    std::string getClassName() { return "SpliceStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    SpliceStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart();
    Tick getEnd();
    
    SpliceStream& addSource(Stream *source);
    SpliceStream& removesSource(Stream *source);
    SpliceStream& removeAllSources();

  protected:

    // ================================================================
    // private instance methods

    void resetCursor();

    void setupCursor(Tick time, Tick end);

    void sortSources();

    static bool sortPredicate(mu::Stream *s0, mu::Stream *s1);

    // ================================================================
    // private instance variables

    Tick prev_tick_;

    long int cursor_;
  };                            // class SpliceStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
