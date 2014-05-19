#include "constant_stream.h"

namespace mu {

  ConstantStream::ConstantStream() 
    : value_ ( 0.0 ) {
    TRACE("ConstantStream::ConstantStream()\n");
  }
  
  ConstantStream::~ConstantStream() {
    TRACE("ConstantStream::~ConstantStream()\n");
  }
  
  void ConstantStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getValue() = " << getValue() << std::endl;
  }
    
  ConstantStream& ConstantStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    for (Tick i=buffer.size()-1; i>=0; i--) { buffer[i] = value_; }
    return *this;
  }

}
