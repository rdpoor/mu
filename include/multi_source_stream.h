/*
 * MultiSourceStream is an abstract superclass for streams that take an
 * arbitrary number of homogeneous sources.
 */
#ifndef MU_MULTI_SOURCE_STREAM
#define MU_MULTI_SOURCE_STREAM

#include "mu.h"
#include "stream.h"

namespace mu {

  class MultiSourceStream : public Stream {
  public:
    virtual MultiSourceStream& step(stk::StkFrames& buffer, Tick tick, Player &player) = 0;
    
    Tick getStart( void );
    Tick getEnd( void );

    MultiSourceStream& addSource(Stream *source);
    MultiSourceStream& removeSource(Stream *source);
    MultiSourceStream& removeAllSources();
    size_t getSourceCount() { return sources_.size(); }

    // Allow readonly access to the underlying sources.  This is
    // primarily intended for unit testing.
    const StreamVector& getSources() const { return sources_; }

  protected:
    StreamVector sources_;
    stk::StkFrames buffer_;
  };

}

#endif

