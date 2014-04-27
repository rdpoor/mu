#ifndef MU_NODE_H
#define MU_NODE_H

#include "mu.h"

namespace mu {

  class Node {
  public:
    const static int INDEFINITE = -1;
    
    //! Write upto nFrames of interleaved sample data into buffer.
    //! Returns number of frames written into the buffer.
    virtual Node& step(stk::StkFrames& buffer, 
                       unsigned int frame_count, 
                       unsigned int channel_count) = 0;
    
    //! reset the Node to time 0, reinitialize state.
    Node& reset();

    //! allocate resources required by this node.
    virtual Node& acquireResources() = 0;

    //! release any resources used by this node
    virtual Node& releaseResources() = 0;

    //! condition the Node so the next call to step() will fetch samples
    //! starting at \c time.
    virtual Node& seek(MuTime time) = 0;

    //! Return the number of frames remaining in this stream, or
    //! INDEFINITE if the stream has an infinite or indeterminate
    //! number of frames.
    virtual long int framesRemaining() = 0;

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
