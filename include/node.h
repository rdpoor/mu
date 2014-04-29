#ifndef MU_NODE_H
#define MU_NODE_H

#include "mu.h"
#include "player.h"
#include <math.h>

namespace mu {

  class Node {
  public:
    
    virtual Node& step(stk::StkFrames& buffer, 
                       Tick tick,
                       const Player &player) = 0;
    
    //! Return the duration of this stream or kIndefinite if the
    //! stream has an infinite or indeterminate duration.
    virtual Tick streamDuration() = 0;

  };                            // class Node

} // namespace mu
#endif
