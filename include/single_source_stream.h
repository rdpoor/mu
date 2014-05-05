/*
 * SingleSourceStream is an abstract superclass for streams that take a
 * single source stream as an input.
 */
#ifndef MU_SINGLE_SOURCE_STREAM
#define MU_SINGLE_SOURCE_STREAM

#include "mu.h"
#include "stream.h"

namespace mu {

  class SingleSourceStream : public Stream {
  public:
    virtual SingleSourceStream& step(stk::StkFrames& buffer, Tick tick, Player &player) = 0;
    virtual Tick getStart( void ) { return (source_ == NULL) ? kIndefinite : source_->getStart(); }
    virtual Tick getEnd( void ) { return (source_ == NULL) ? kIndefinite : source_->getEnd(); }
    virtual Stream *getSource() { return source_; }

  protected:
    Stream *source_;
    stk::StkFrames buffer_;
  };

}

#endif

