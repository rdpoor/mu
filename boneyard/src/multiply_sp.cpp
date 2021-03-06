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
#include "multiply_sp.h"

namespace mu {

  MultiplySP::MultiplySP() {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  MultiplySP::~MultiplySP() {
  }

  void MultiplySP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Inputs:" << std::endl;
    for (int i=0; i<sources_.size(); i++) {
      ss << sources_.at(i)->inspect(level+1);
    }
  }
    
  // TODO: should we write ones rather than zeros?
  void MultiplySP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (sources_.size() == 0) {
      zeroBuffer(buffer);
    } else {
      // first source is written directly.
      SampleProcessor *source = sources_.at(0);
      source->step(buffer, tick, is_new_event);
      for (long int i=1; i<sources_.size(); i++) {
        source = sources_.at(i);
        Tick buffer_start = tick;
        Tick buffer_end = tick + buffer.frames();
        Tick source_start = source->getStart();
        Tick source_end = source->getEnd();
        // TODO: write general SP::is_overlapping() method
        if ((TickUtils::isIndefinite(source_start) || (source_start < buffer_end)) &&
            (TickUtils::isIndefinite(source_end) || (source_end > buffer_start))) {
          buffer_.resize(buffer.frames(), buffer.channels());
          source->step(buffer_, tick, is_new_event);
          stk::StkFloat *dst = &(buffer[0]);
          stk::StkFloat *src = &(buffer_[0]);
          for (int i=buffer.size()-1; i>=0; i--) { *dst++ *= *src++; }
        }
      }
    }
  }

}
