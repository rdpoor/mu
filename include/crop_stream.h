/*
 * CropStream limits a stream to a definite starting and ending time,
 * returning zero samples ouside of those times.  start and/or end
 * may be set to kIndefinite, in which case CropStream inherits the
 * corresponding start or end time from the source.
 */
#ifndef MU_CROP_STREAM_H
#define MU_CROP_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class CropStream : public SingleSourceStream {
  public:

    CropStream( void );
    ~CropStream( void );

    std::string getClassName() { return "CropStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    CropStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    // needed(??) so setSource() returns a LoopStream and not a SingleSourceStream
    // TODO: figure out the right way to do this
    CropStream& setSource(Stream *source) { source_ = source; return *this; }

    Tick getStart( void );
    CropStream& setStart(Tick start) {  start_ = start; return *this; }

    Tick getEnd( void );
    CropStream& setEnd(Tick end) { end_ = end; return *this; }


  protected:
    Tick start_;
    Tick end_;
  };                            // class CropStream
  
}                               // namespace mu

#endif
