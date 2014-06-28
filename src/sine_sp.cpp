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
#include "sine_sp.h"
#include <math.h>

namespace mu {

  SineSP::SineSP() 
    : amplitude_ ( 1.0 ),
      frequency_ ( 440.0 ),
      phase_ ( 0.0 ) {
  }
  
  SineSP::~SineSP() {
  }
  
  void SineSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getAmplitude() = " << getAmplitude() << std::endl;
    inspectIndent(ss, level); ss << "getFrequency() = " << getFrequency() << std::endl;
    inspectIndent(ss, level); ss << "getPhase() = " << getPhase() << std::endl;
  }
    
  void SineSP::step(stk::StkFrames& buffer, Tick tick, Player &player) {

    // fprintf(stderr,"SineSP::step(tick=%ld)\n",tick);

    if (buffer.channels() == 1) {
      for (Tick i=0; i<buffer.frames(); i++) { 
        double t = (double)(tick + i)/buffer.dataRate();
        double value = amplitude_ * sin(t * 2.0 * M_PI * frequency_ + phase_);
        buffer(i,0) = value;
      }
    } else if (buffer.channels() == 2) {
      for (Tick i=0; i<buffer.frames(); i++) { 
        double t = (double)(tick + i)/buffer.dataRate();
        double value = amplitude_ * sin(t * 2.0 * M_PI * frequency_ + phase_);
        buffer(i,0) = value;
        buffer(i,1) = value;
      }
    } else {
      for (Tick i=0; i<buffer.frames(); i++) { 
        double t = (double)(tick + i)/buffer.dataRate();
        double value = amplitude_ * sin(t * 2.0 * M_PI * frequency_ + phase_);
        for (int j = 0; j < buffer.channels(); j++) {
          buffer(i,j) = value;
        }
      }
    }

  }

}
