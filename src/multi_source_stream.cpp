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
#include "multi_source_stream.h"

namespace mu {

  Tick MultiSourceStream::getStart() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      Tick earliest_start = LONG_MAX;
      for (long int i=0; i<sources_.size(); i++) {
        Stream *source = sources_.at(i);
        Tick start = source->getStart();
        if (start == kIndefinite) {
          return kIndefinite;
        } else {
          earliest_start = std::min(earliest_start, start);
        }
      }
      return earliest_start;
    }
  }

  Tick MultiSourceStream::getEnd() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      Tick latest_end = LONG_MIN;
      for (long int i=0; i<sources_.size(); i++) {
        Stream *source = sources_.at(i);
        Tick end = source->getEnd();
        if (end == kIndefinite) {
          return kIndefinite;
        } else {
          latest_end = std::max(latest_end, end);
        }
      }
      return latest_end;
    }
  }

  MultiSourceStream& MultiSourceStream::addSource(Stream *source) {
    sources_.push_back(source);
    return *this;
  }
  
  MultiSourceStream& MultiSourceStream::removeSource(Stream *source) {
    for (int i=0; i<sources_.size(); i++) {
      if (sources_.at(i) == source) {
        sources_.erase(sources_.begin()+i);
        return *this;
      }
    }
    return *this;
  }
  
  MultiSourceStream& MultiSourceStream::removeAllSources() {
    sources_.clear();
    return *this;
  }
  
  
}
