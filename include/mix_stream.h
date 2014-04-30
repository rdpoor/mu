/*
 * MixStream mixes two sources together.
 * TODO: mix N sources together.
 */
#ifndef MU_MIX_STREAM_H
#define MU_MIX_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class MixStream : public Stream {
  public:

    MixStream();
    ~MixStream( void );

    MixStream& step(stk::StkFrames& buffer, 
                          Tick tick,
                          Player &player);
    Tick frameCount();

    Stream *getSourceA() const;
    MixStream& setSourceA(Stream *source);

    Stream *getSourceB() const;
    MixStream& setSourceB(Stream *source);

  protected:
    Stream *source_a_;
    Stream *source_b_;
    stk::StkFrames buffer_;

  };                            // class MixStream

  inline MixStream::MixStream() {
    TRACE("MixStream::MixStream()\n");
  }

  inline Stream *MixStream::getSourceA() const { return source_a_; }
  inline MixStream& MixStream::setSourceA(Stream *source) {
    TRACE("MixStream::setSourceA()\n");
    source_a_ = source; return *this;
  }
  
  inline Stream *MixStream::getSourceB() const { return source_b_; }
  inline MixStream& MixStream::setSourceB(Stream *source) {
    TRACE("MixStream::setSourceB()\n");
    source_b_ = source; return *this;
  }
  
}                               // namespace mu

#endif
