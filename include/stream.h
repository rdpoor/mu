#ifndef MU_STREAM_H
#define MU_STREAM_H

#include "mu.h"
#include "player.h"
#include <math.h>

namespace mu {

  class Stream {
  public:
    
    virtual Stream& step(stk::StkFrames& buffer, 
                       Tick tick,
                       Player &player) = 0;
    
    //! Return the duration of this stream or kIndefinite if the
    //! stream has an infinite or indeterminate duration.
    virtual Tick frameCount() = 0;

  };                            // class Stream

} // namespace mu
#endif
