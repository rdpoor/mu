/*
 * MixNStream mixes N sources together.
 */
#ifndef MU_MIX_N_STREAM_H
#define MU_MIX_N_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  typedef std::vector<mu::Stream *> StreamVector;

  class MixNStream : public Stream {
  public:

    MixNStream();
    ~MixNStream( void );

    MixNStream& step(stk::StkFrames& buffer, 
                          Tick tick,
                          Player &player);
    Tick getStart( void );
    Tick getEnd( void );

    MixNStream& addSource(Stream *source);
    MixNStream& removeSource(Stream *source);

  protected:
    StreamVector sources_;
    stk::StkFrames buffer_;

  };                            // class MixNStream

}                               // namespace mu

#endif
