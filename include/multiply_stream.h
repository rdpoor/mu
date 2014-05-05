/*
 * MultiplyStream takes the product of N sources.
 */
#ifndef MU_MULTIPLY_STREAM_H
#define MU_MULTIPLY_STREAM_H

#include "mu.h"
#include "multi_source_stream.h"

namespace mu {

  class MultiplyStream : public MultiSourceStream {
  public:

    MultiplyStream( void );
    ~MultiplyStream( void );
    MultiplyStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

  };                            // class MultiplyStream

}                               // namespace mu

#endif
