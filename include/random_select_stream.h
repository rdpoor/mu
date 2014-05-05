/*
 * RandomSelectStream accepts an arbitrary number of input streams.
 * One stream is selected as the "current" stream.  In each call to
 * step(), if the tick time goes backwards, another stream is randomly
 * selected as the current stream.
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
    RandomSelectStream& step(stk::StkFrames& buffer, Tick tick, Player &player) {
      if (sources_.size() == 0) {
        zero_buffer(buffer);
      } else {
        if ((tick < prev_tick_) || (current_stream_ == NULL)) { reset(); }
        prev_tick_ = tick;
        current_stream_->step(buffer, tick, player);
      }
      return *this;
    }
      
    Tick getStart() { 
      return (current_stream_ == NULL) ? kIndefinite : current_stream_->getStart(); 
    }
    
    Tick getEnd() { 
      return (current_stream_ == NULL) ? kIndefinite : current_stream_->getEnd(); 
    }
    
  protected:
    RandomSelectStream& reset() {
      int index = std::rand() % sources_.size();
      current_stream_ = sources_.at(index);
      return *this;
    }
    
    Tick prev_tick_;
    Stream *current_stream_;
  };                            // class RandomSelectStream

  inline RandomSelectStream::RandomSelectStream()
    : prev_tick_ (kIndefinite),
    current_stream_ (NULL) {
  }

  inline RandomSelectStream::~RandomSelectStream() { }

}                               // namespace mu

#endif
