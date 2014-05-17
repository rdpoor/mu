/*
 * CropStream limits a stream to a definite starting and ending time,
 * returning zero samples ouside of those times.  start and/or end
 * may be set to kIndefinite, in which case CropStream inherits the
 * corresponding start or end time from the source.
 */
#ifndef MU_X_FADE_STREAM_H
#define MU_X_FADE_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class XFadeStream : public SingleSourceStream {
  public:

    XFadeStream() : 
      start_ (kIndefinite), 
      end_ (kIndefinite),
      x_fade_time_ ( 0 ) {
    }
    ~XFadeStream( void );
    std::string getClassName() { return "XFadeStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    XFadeStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    XFadeStream& setSource(Stream *source) { source_ = source; return *this; }

    Tick getStart( void );
    XFadeStream& setStart(Tick start) {  start_ = start; return *this; }

    Tick getEnd( void );
    XFadeStream& setEnd(Tick end) { end_ = end; return *this; }

    Tick getXFadeTime( void ) { return x_fade_time_; }
    XFadeStream& setXFadeTime(Tick x_fade_time) { 
      x_fade_time_ = x_fade_time; 
      return *this; 
    }

  protected:
    Tick start_;
    Tick end_;
    Tick x_fade_time_;
  };                            // class XFadeStream

  
}                               // namespace mu

#endif
