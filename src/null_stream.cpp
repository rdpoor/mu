#include "null_stream.h"

namespace mu {

  NullStream::NullStream( void ) { }
  NullStream::~NullStream( void ) { }

  NullStream& NullStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    zeroBuffer(buffer);
    return *this;
  }

}
