#include "sine_stream.h"
#include <math.h>

namespace mu {

  SineStream::SineStream() 
    : amplitude_ ( 1.0 ),
      frequency_ ( 440.0 ),
      phase_ ( 0.0 ) {
    TRACE("SineStream::SineStream()\n");
  }
  
  SineStream::~SineStream() {
    TRACE("SineStream::~SineStream()\n");
  }
  
  void SineStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getAmplitude() = " << getAmplitude() << std::endl;
    inspectIndent(ss, level); ss << "getFrequency() = " << getFrequency() << std::endl;
    inspectIndent(ss, level); ss << "getPhase() = " << getPhase() << std::endl;
  }
    
  SineStream& SineStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {

    // fprintf(stderr,"SineStream::step(tick=%ld)\n",tick);

    for (Tick i=buffer.size()-1; i>=0; i--) { 
      double t = (double)(tick + i)/buffer.dataRate();
      double value = amplitude_ * sin(t * 2.0 * M_PI * frequency_ + phase_);
      for (int j = 0; j < buffer.channels(); j++) {
        buffer(i,j) = value;
      }
    }
    return *this;
  }

}
