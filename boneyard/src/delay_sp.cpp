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
#include "delay_sp.h"

namespace mu {

  DelaySP::DelaySP() :
    delay_ ( kDefaultDelay ) {
  }

  DelaySP::~DelaySP() {
  }

  void DelaySP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getDelay() = " << getDelay() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void DelaySP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      source_->step(buffer, tick - delay_, is_new_event);
    }
  }

  Tick DelaySP::getStart() {
    return (source_ == NULL) ? TickUtils::indefinite() : TickUtils::sum(source_->getStart(), delay_);
  }

  Tick DelaySP::getEnd() {
    return (source_ == NULL) ? TickUtils::indefinite() : TickUtils::sum(source_->getEnd(), delay_);
  }

}
