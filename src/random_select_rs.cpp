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
#include "random_select_rs.h"

namespace mu {
  
  RandomSelectRS::RandomSelectRS()
    : current_stream_ (NULL) {
  }
  
  RandomSelectRS::~RandomSelectRS() { 
  }

  void RandomSelectRS::render(stk::StkFrames& frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (start_tick == 0) {
      reset();
    }
    if (current_stream_ != NULL) {
      current_stream_->render(frames, base_tick, start_tick, end_tick);
    }
  }
    
  RandomSelectRS& RandomSelectRS::reset() {
    if (sources_.size() > 0) {
      int index = std::rand() % sources_.size();
      current_stream_ = sources_.at(index);
    } else {
      current_stream_ = NULL;
    }
    return *this;
  }
    
  
}
