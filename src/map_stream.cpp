#include "map_stream.h"

namespace mu {

  MapStream::~MapStream() {
    TRACE("MapStream::~MapStream()\n");
  }

  MapStream& MapStream::step(stk::StkFrames& buffer,
                             Tick tick,
                             Player& player) {
    // fill buffer if there's a source
    if (source_ != NULL) {
      source_->step(buffer, tick, player);
    }

    // call callback on each frame to create or analyze frame data
    if (callback_ != NULL) {
      for (Tick i=0; i<buffer.frames(); i++) { 
        callback_(buffer, tick+i, player, i); 
      }
    }

    return *this;
  }

  // stream extent is inherited from the source stream
  Tick MapStream::getStart() {
    return (source_ == NULL) ? kIndefinite : (source_->getStart());
  }

  Tick MapStream::getEnd() {
    return (source_ == NULL) ? kIndefinite : (source_->getEnd());
  }


}
