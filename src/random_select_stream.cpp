#include "random_select_stream.h"

namespace mu {
  
  RandomSelectStream::RandomSelectStream()
    : prev_tick_ (kIndefinite),
      current_stream_ (NULL) {
  }
  
  RandomSelectStream::~RandomSelectStream() { 
  }

  RandomSelectStream& RandomSelectStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    if (sources_.size() == 0) {
      zeroBuffer(buffer);
    } else {
      if ((tick < prev_tick_) || (current_stream_ == NULL)) { reset(); }
      prev_tick_ = tick;
      current_stream_->step(buffer, tick, player);
    }
    return *this;
  }

  Tick RandomSelectStream::getStart() { 
    return (current_stream_ == NULL) ? kIndefinite : current_stream_->getStart(); 
  }
  
  Tick RandomSelectStream::getEnd() { 
    return (current_stream_ == NULL) ? kIndefinite : current_stream_->getEnd(); 
  }
    
  RandomSelectStream& RandomSelectStream::reset() {
    int index = std::rand() % sources_.size();
    current_stream_ = sources_.at(index);
    return *this;
  }
    
  
}
