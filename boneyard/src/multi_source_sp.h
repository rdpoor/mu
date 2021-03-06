/*
 * MultiSourceSP is an abstract superclass for streams that take an
 * arbitrary number of homogeneous sources.
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

#ifndef MU_MULTI_SOURCE_STREAM
#define MU_MULTI_SOURCE_STREAM

#include "mu.h"
#include "tick_utils.h"
#include "sample_processor.h"

namespace mu {

  template <typename T> class MultiSourceSP : public SampleProcessor {
  public:
    
    Tick getStart( void ) {
      if (sources_.size() == 0) {
        return TickUtils::indefinite();
      } else {
        Tick earliest_start = LONG_MAX;
        for (long int i=0; i<sources_.size(); i++) {
          SampleProcessor *source = sources_.at(i);
          Tick start = source->getStart();
          if (TickUtils::isIndefinite(start)) {
            return TickUtils::indefinite();
          } else {
            earliest_start = std::min(earliest_start, start);
          }
        }
        return earliest_start;
      }
    }
    
    Tick getEnd( void ) {
      if (sources_.size() == 0) {
        return TickUtils::indefinite();
      } else {
        Tick latest_end = LONG_MIN;
        for (long int i=0; i<sources_.size(); i++) {
          SampleProcessor *source = sources_.at(i);
          Tick end = source->getEnd();
          if (TickUtils::isIndefinite(end)) {
            return TickUtils::indefinite();
          } else {
            latest_end = std::max(latest_end, end);
          }
        }
        return latest_end;
      }
    }

    T& addSource(SampleProcessor *source) {
      sources_.push_back(source);
      return *static_cast<T *>(this);
    }
  
    T& removeSource(SampleProcessor *source) {
      for (int i=0; i<sources_.size(); i++) {
        if (sources_.at(i) == source) {
          sources_.erase(sources_.begin()+i);
          break;
        }
      }
      return *static_cast<T *>(this);
    }
    
    T& removeAllSources() {
      sources_.clear();
      return *static_cast<T *>(this);
    }
    
    size_t getSourceCount() { return sources_.size(); }

    // Allow readonly access to the underlying sources.  This is
    // primarily intended for unit testing.
    const SPVector& getSources() const { return sources_; }

  protected:
    SPVector sources_;
    stk::StkFrames buffer_;
  };

}

#endif


// Local Variables:
// mode: c++
// End:
