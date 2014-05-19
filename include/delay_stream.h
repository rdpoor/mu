/*
 * DelayStream delays source frames by a constant number of frames.
 */
#ifndef MU_DELAY_STREAM_H
#define MU_DELAY_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class DelayStream : public SingleSourceStream {
  public:

    DelayStream( void );
    ~DelayStream( void );

    std::string getClassName() { return "DelayStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    DelayStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart( void );
    Tick getEnd( void );

    // needed(??) so setSource() returns a DelayStream and not a SingleSourceStream
    // TODO: figure out the right way to do this
    DelayStream& setSource(Stream *source) { source_ = source; return *this; }

    Tick getDelay() const { return delay_; }
    DelayStream& setDelay(Tick delay) { delay_ = delay; return *this; }

  protected:
    Tick delay_;

  };                            // class DelayStream

}                               // namespace mu

#endif
