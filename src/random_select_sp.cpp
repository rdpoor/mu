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
#include "random_select_sp.h"

namespace mu {
  
  RandomSelectSP::RandomSelectSP()
    : prev_tick_ (kIndefinite),
      current_stream_ (NULL) {
  }
  
  RandomSelectSP::~RandomSelectSP() { 
  }

  void RandomSelectSP::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    if (sources_.size() == 0) {
      zeroBuffer(buffer);
    } else {
      if ((tick < prev_tick_) || (current_stream_ == NULL)) { reset(); }
      prev_tick_ = tick;
      current_stream_->step(buffer, tick, player);
    }
  }

  Tick RandomSelectSP::getStart() { 
    return (current_stream_ == NULL) ? kIndefinite : current_stream_->getStart(); 
  }
  
  Tick RandomSelectSP::getEnd() { 
    return (current_stream_ == NULL) ? kIndefinite : current_stream_->getEnd(); 
  }
    
  RandomSelectSP& RandomSelectSP::reset() {
    int index = std::rand() % sources_.size();
    current_stream_ = sources_.at(index);
    return *this;
  }
    
  
}
