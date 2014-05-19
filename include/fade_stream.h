/*
 * FadeStream fades the source stream in and out.  
 *
 * If getStart() is indefinite, fade in starts at source->getStart().
 * If getStart() is definite, then fade in starts at getStart() -
 * getFadeTime() / 2 or at source->getStart(), whichever is greater.
 *
 * If getEnd() is indefinite, fade out ends at source->getEnd().
 * If getEnd() is definite, then fade out ends at getStart() +
 * getFadeTime() / 2 or at source->getEnd(), whichever is less.

 */
#ifndef MU_FADE_STREAM_H
#define MU_FADE_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class FadeStream : public SingleSourceStream {
  public:

    FadeStream( void );
    ~FadeStream( void );

    std::string getClassName() { return "FadeStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    FadeStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart( void );
    FadeStream& setStart(Tick start) {  start_ = start; return *this; }

    Tick getEnd( void );
    FadeStream& setEnd(Tick end) { end_ = end; return *this; }

    FadeStream& setSource(Stream *source) { source_ = source; return *this; }

    Tick getFadeTime( void ) { return fade_time_; }
    FadeStream& setFadeTime(Tick fade_time) { fade_time_ = fade_time; return *this; }

  protected:
    Tick start_;
    Tick end_;
    Tick fade_time_;
  };                            // class FadeStream

  
}                               // namespace mu

#endif
