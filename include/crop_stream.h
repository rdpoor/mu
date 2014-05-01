/*
 * CropStream limits a stream to a definite starting and ending time,
 * returning zero samples ouside of those times.  start and/or end
 * may be set to kIndefinite, in which case CropStream inherits the
 * corresponding start or end time from the source.
 */
#ifndef MU_CROP_STREAM_H
#define MU_CROP_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class CropStream : public Stream {
  public:

    CropStream() 
      : start_ (kIndefinite), end_ (kIndefinite) {
      TRACE("CropStream::CropStream()\n");
    }

    ~CropStream( void );

    CropStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart( void );
    CropStream& setStart(Tick start) {  start_ = start; return *this; }
    Tick getEnd( void );
    CropStream& setEnd(Tick end) { end_ = end; return *this; }

    Stream *getSource() const { return source_; }

    CropStream& setSource(Stream *source) {
      TRACE("CropStream::source()\n");
      source_ = source; return *this;
    }


  protected:
    Stream *source_;
    Tick start_;
    Tick end_;
    stk::StkFrames buffer_;

  };                            // class CropStream

  
}                               // namespace mu

#endif
