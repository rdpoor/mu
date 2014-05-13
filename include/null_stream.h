/*
 * NullStream is a source of zeros.
 */
#ifndef MU_NULL_STREAM_H
#define MU_NULL_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class NullStream : public Stream {
  public:

    NullStream( void ) { }
    ~NullStream( void ) { }
    NullStream& step(stk::StkFrames& buffer, Tick tick, Player& player) {
      zero_buffer(buffer);
      return *this;
    }
    
  protected:
  };                            // class NullStream
}                               // namespace mu
#endif
