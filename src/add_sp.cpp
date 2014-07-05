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
#include "add_sp.h"

namespace mu {

  AddSP::AddSP() {
    TRACE("AddSP::AddSP()\n");
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  AddSP::~AddSP() {
    TRACE("AddSP::~AddSP()\n");
  }

  void AddSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Inputs:" << std::endl;
    for (int i=0; i<sources_.size(); i++) {
      ss << sources_.at(i)->inspect(level+1);
    }
  }
    
  void AddSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    int active_streams = 0;
    Tick buffer_start = tick;
    Tick buffer_end = tick + buffer.frames();

    // fprintf(stderr,"AddSP::step(tick=%ld)\n",tick);

    for (int i=sources_.size()-1; i>=0; i--) {
      SampleProcessor *source = sources_.at(i);
      Tick source_start = source->getStart();
      Tick source_end = source->getEnd();
      if (((source_start == TickUtils::indefinite()) || (source_start < buffer_end)) &&
          ((source_end == TickUtils::indefinite()) || (source_end > buffer_start))) {
        // source has one or more frames to contribute to output.
        if (active_streams == 0) {
          // the first source can be copied directly to output.
          source->step(buffer, tick, is_new_event);
        } else {
          // subsequent sources must be fetched into a holding buffer
          // and summed into the output.
          buffer_.resize(buffer.frames(), buffer.channels());
          source->step(buffer_, tick, is_new_event);
#if 0
          stk::StkFloat *dst = &(buffer[0]);
          stk::StkFloat *src = &(buffer_[0]);
          for (int j=buffer.size()-1; j>=0; j--) { *dst++ += *src++; }
#else
          for (int i=0; i<buffer.frames(); i++) {
            for (int j=0; j<buffer.channels(); j++) {
              buffer(i,j) += buffer_(i,j);
            }
          }
#endif
        }
        active_streams += 1;
      }
    }

    // if no stream has contributed to output, explicitly zero it
    if (active_streams == 0) { zeroBuffer(buffer); }
  }

}
