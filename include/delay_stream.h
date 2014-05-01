/*
 * DelayStream delays source frames by a constant number of frames.
 */
#ifndef MU_DELAY_STREAM_H
#define MU_DELAY_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class DelayStream : public Stream {
  public:

    DelayStream();
    ~DelayStream( void );

    DelayStream& step(stk::StkFrames& buffer, 
                          Tick tick,
                          Player &player);
    Tick getStart( void );
    Tick getEnd( void );

    Stream *getSource() const;
    DelayStream& setSource(Stream *source);

    Tick getDelay() const;
    DelayStream& setDelay(Tick delay);

  protected:
    Stream *source_;
    Tick delay_;

  };                            // class DelayStream

  inline DelayStream::DelayStream() {
    TRACE("DelayStream::DelayStream()\n");
  }

  inline Stream *DelayStream::getSource() const { return source_; }
  inline DelayStream& DelayStream::setSource(Stream *source) {
    TRACE("DelayStream::source()\n");
    source_ = source; return *this;
  }
  
  inline Tick DelayStream::getDelay() const { return delay_; }
  inline DelayStream& DelayStream::setDelay(Tick delay) {
    TRACE("DelayStream::setCallback()\n");
    delay_ = delay; return *this;
  }

}                               // namespace mu

#endif
