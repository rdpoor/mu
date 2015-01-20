/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
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
 * MultiSourceStream is an abstract superclass for streams that take an
 * arbitrary number of sources.
 */

#ifndef MU_MULTI_SOURCE_STREAM_H
#define MU_MULTI_SOURCE_STREAM_H

#include "mu_stream.h"
#include "mu_types.h"

namespace mu {

  class MultiSourceStream : public MuStream {
  public:
    
    void add_source(MuStream *source) {
      sources_.push_back(source);
    }
  
    void remove_source(MuStream *source) {
      for (int i=0; i<sources_.size(); i++) {
        if (sources_.at(i) == source) {
          sources_.erase(sources_.begin()+i);
          break;
        }
      }
    }
    
    void remove_all_sources() {
      sources_.clear();
    }
    
    size_t source_count() { return sources_.size(); }

    // Allow readonly access to the underlying sources, primarily intended for
    // unit testing.
    const MuStreamVector& sources() const { return sources_; }

  protected:
    MuStreamVector sources_;
    MuBuffer tmp_buffer_;
  };

}

#endif


// Local Variables:
// mode: c++
// End:
