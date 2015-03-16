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
#include "probability_sp.h"

namespace mu {

  ProbabilitySP::ProbabilitySP( void )
    : current_stream_ ( NULL ),
      probability_ (1.0) {
  }

  ProbabilitySP::~ProbabilitySP( void ) { 
  }

  void ProbabilitySP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    if (is_new_event) { reset(); }
    
    if (current_stream_ == NULL ) {
      zeroBuffer(buffer);
    } else {
      current_stream_->step(buffer, tick, is_new_event);
    }
  }

  Tick ProbabilitySP::getStart() {
    return TickUtils::indefinite();
  }

  Tick ProbabilitySP::getEnd() {
    return TickUtils::indefinite();
  }

  // Arrive here when the stream starts or backs up: flip a weighted
  // coin to choose whether or not to play the source stream.
  ProbabilitySP& ProbabilitySP::reset() {
    double p = ((double) rand() / (RAND_MAX));
    current_stream_ = (p < probability_) ? getSource() : NULL;
    return *this;
  }
  
}
