/*
 * RandomSelectStream accepts an arbitrary number of input streams.
 * One stream is selected as the "current" stream.  In each call to
 * step(), if the tick time goes backwards, another stream is randomly
 * selected as the current stream.
 *
 * Note that each input stream could have a different delay stream
 * (to randomize start times) or different sound files (to randomize
 * the sound played), etc.
 */
#ifndef MU_RANDOM_SELECT_STREAM_H
#define MU_RANDOM_SELECT_STREAM_H

#include "mu.h"
#include "multi_source_stream.h"

namespace mu {

  class RandomSelectStream : public MultiSourceStream {
  public:
    
    RandomSelectStream( void );
    ~RandomSelectStream( void );

    std::string getClassName() { return "RandomSelectStream"; }

    RandomSelectStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart();
    Tick getEnd();
    
  protected:
    RandomSelectStream& reset();
    Tick prev_tick_;
    Stream *current_stream_;
  };                            // class RandomSelectStream

}                               // namespace mu

#endif
