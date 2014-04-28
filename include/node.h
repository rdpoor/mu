#ifndef MU_NODE_H
#define MU_NODE_H

#include "mu.h"
#include "transport.h"
#include <math.h>

namespace mu {

  class Node {
  public:
    // return value for streamDuration() signifying indefinite length
    // stream...
    // static constexpr MuTime kIndefinite = -MAXFLOAT;
    #define INDEFINITE (-MAXFLOAT)
    
    virtual Node& step(stk::StkFrames& buffer, 
                       MuTime time,
                       const Transport &transport) = 0;
    
    //! Return the duration of this stream or INDEFINITE if the stream
    //! has an infinite or indeterminate duration.
    virtual MuTime streamDuration() = 0;

  };                            // class Node

} // namespace mu
#endif
