/*
 * LoopStream takes an input stream and causes it to repeat every
 * loopDuration seconds.
 */
#ifndef MU_LOOP_STREAM_H
#define MU_LOOP_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class LoopStream : public SingleSourceStream {
  public:

    LoopStream();
    ~LoopStream( void );

    LoopStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    // needed(??) so setSource() returns a LoopStream and not a SingleSourceStream
    // TODO: figure out the right way to do this
    LoopStream& setSource(Stream *source) { source_ = source; return *this; }

    Tick getLoopDuration() const;
    LoopStream& setLoopDuration(Tick loop_duration);

  protected:
    Tick loop_duration_;

  };                            // class LoopStream

  inline LoopStream::LoopStream()
    : loop_duration_ (44100) {
    TRACE("LoopStream::LoopStream()\n");
  }

  inline Tick LoopStream::getLoopDuration() const { return loop_duration_; }
  inline LoopStream& LoopStream::setLoopDuration(Tick loop_duration) {
    TRACE("LoopStream::loopDuration()\n");
    loop_duration_ = loop_duration; return *this;
  }

}                               // namespace mu

#endif
