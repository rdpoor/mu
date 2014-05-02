#include "constant_stream.h"

namespace mu {

  ConstantStream::ConstantStream() {
    TRACE("ConstantStream::ConstantStream()\n");
  }
  
  ConstantStream::~ConstantStream() {
    TRACE("ConstantStream::~ConstantStream()\n");
  }
  
  ConstantStream& ConstantStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    for (Tick i=buffer.size()-1; i>=0; i--) { buffer[i] = value_; }
    return *this;
  }

}
