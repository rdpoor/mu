#include "probability_stream.h"

namespace mu {

  ProbabilityStream::ProbabilityStream( void )
    : prev_tick_  (kIndefinite),
      current_stream_ ( NULL ),
      probability_ (1.0) {
  }

  ProbabilityStream::~ProbabilityStream( void ) { 
  }

  ProbabilityStream& ProbabilityStream::step(stk::StkFrames& buffer, Tick tick, Player &player) {
    if ((prev_tick_ == kIndefinite) || (tick <= prev_tick_)) { reset(); }
    prev_tick_ = tick;
    
    if (current_stream_ == NULL ) {
      zeroBuffer(buffer);
    } else {
      current_stream_->step(buffer, tick, player);
    }
    return *this;
  }

  Tick ProbabilityStream::getStart() {
    return kIndefinite;
  }

  Tick ProbabilityStream::getEnd() {
    return kIndefinite;
  }

  // Arrive here when the stream starts or backs up: flip a weighted
  // coin to choose whether or not to play the source stream.
  ProbabilityStream& ProbabilityStream::reset() {
    double p = ((double) rand() / (RAND_MAX));
    current_stream_ = (p < probability_) ? getSource() : NULL;
    return *this;
  }
  
}
