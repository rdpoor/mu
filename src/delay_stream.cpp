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
#include "delay_stream.h"

namespace mu {

  DelayStream::DelayStream() 
    : delay_ ( kDefaultDelay ) {
    TRACE("DelayStream::DelayStream()\n");
  }

  DelayStream::~DelayStream() {
    TRACE("DelayStream::~DelayStream()\n");
  }

  void DelayStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getDelay() = " << getDelay() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void DelayStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      source_->step(buffer, tick - delay_, player);
    }
  }

  Tick DelayStream::getStart() {
    if ((source_ == NULL) || (source_->getStart() == kIndefinite)) {
      return kIndefinite;
    } else {
      return (source_->getStart() + delay_);
    }
  }

  Tick DelayStream::getEnd() {
    if ((source_ == NULL) || (source_->getEnd() == kIndefinite)) {
      return kIndefinite;
    } else {
      return (source_->getEnd() + delay_);
    }
  }

}
