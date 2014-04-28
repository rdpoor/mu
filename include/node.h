#ifndef MU_NODE_H
#define MU_NODE_H

#include "mu.h"
#include "transport.h"
#include <math.h>

namespace mu {

  class Node {
  public:
    // return value for duration() signifying indefinite length
    // stream...
    // static constexpr MuTime kIndefinite = -MAXFLOAT;
    #define INDEFINITE (-MAXFLOAT)
    
    virtual Node& step(stk::StkFrames& buffer, 
                       MuTime time,
                       const Transport &transport) = 0;
    
    //! reset the Node to time 0, reinitialize state.
    Node& reset();

    //! allocate resources required by this node.
    virtual Node& acquireResources() = 0;

    //! release any resources used by this node
    virtual Node& releaseResources() = 0;

    //! condition the Node so the next call to step() will fetch samples
    //! starting at \c time.
    virtual Node& seek(MuTime time) = 0;

    //! Return the duration of this stream or INDEFINITE if the stream
    //! has an infinite or indeterminate duration.
    virtual MuTime duration() = 0;

  };                            // class Node

  inline Node& Node::reset() {
    TRACE("Node::reset()\n");
    releaseResources();
    acquireResources();
    seek(0.0);
    return *this;
  }
  
} // namespace mu
#endif
