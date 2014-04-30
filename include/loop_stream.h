/*
 * LoopStream takes an input stream and causes it to repeat every
 * loopDuration seconds.
 */
#ifndef MU_LOOP_STREAM_H
#define MU_LOOP_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class LoopStream : public Stream {
  public:

    LoopStream();
    ~LoopStream( void );

    LoopStream& step(stk::StkFrames& buffer, 
                 Tick tick,
                 Player &player);
    Tick frameCount();

    Tick getLoopDuration() const;
    LoopStream& setLoopDuration(Tick loop_duration);

    Stream *getSource() const;
    LoopStream& setSource(Stream *source);

  protected:
    Tick loop_duration_;
    stk::StkFrames buffer_;
    Stream *source_;

  };                            // class LoopStream

  inline LoopStream::LoopStream()
    : loop_duration_ (1.0) {
    TRACE("LoopStream::LoopStream()\n");
  }

  inline Tick LoopStream::frameCount() { return kIndefinite; }

  inline Tick LoopStream::getLoopDuration() const { return loop_duration_; }
  inline LoopStream& LoopStream::setLoopDuration(Tick loop_duration) {
    TRACE("LoopStream::loopDuration()\n");
    loop_duration_ = loop_duration; return *this;
  }

  inline Stream *LoopStream::getSource() const { return source_; }
  inline LoopStream& LoopStream::setSource(Stream *source) {
    TRACE("LoopStream::source()\n");
    source_ = source; return *this;
  }
  

}                               // namespace mu

#endif
