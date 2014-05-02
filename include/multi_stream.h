/*
 * Multi-stream is an abstract superclass for streams that take an
 * arbitrary number of homogeneous sources.
 */
#ifndef MU_MULTI_STREAM
#define MU_MULTI_STREAM

#include "mu.h"
#include "stream.h"

namespace mu {

  class MultiStream : public Stream {
  public:
    virtual MultiStream& step(stk::StkFrames& buffer, 
                              Tick tick,
                              Player &player) = 0;
    
    Tick getStart( void );
    Tick getEnd( void );

    MultiStream& addSource(Stream *source);
    MultiStream& removeSource(Stream *source);
    MultiStream& removeAllSources();

  protected:
    StreamVector sources_;
    stk::StkFrames buffer_;
  };

}

#endif

