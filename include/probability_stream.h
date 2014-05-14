/*
 * ProbabilityStream plays its source stream with a given \c
 * probability whenever the stream is started or backs up.
 */
#ifndef MU_PROBABILITY_STREAM_H
#define MU_PROBABILITY_STREAM_H

#include "mu.h"
#include "single_source_stream.h"

namespace mu {

  class ProbabilityStream : public SingleSourceStream {
  public:
    ProbabilityStream( void ) : 
      prev_tick_  (kIndefinite),
      current_stream_ ( NULL ),
      probability_ (1.0) {
    }
    ~ProbabilityStream( void ) { }
    std::string getClassName() { return "ProbabilityStream"; }
  
    ProbabilityStream& step(stk::StkFrames& buffer, Tick tick, Player &player) {
      if ((prev_tick_ == kIndefinite) || (tick <= prev_tick_)) { reset(); }
      prev_tick_ = tick;

      if (current_stream_ == NULL ) {
        zero_buffer(buffer);
      } else {
        current_stream_->step(buffer, tick, player);
      }
      return *this;
    }

    Tick getStart( void ) { return kIndefinite; }
    Tick getEnd( void ) { return kIndefinite; }

    ProbabilityStream& setSource(Stream *source) { source_ = source; return *this; }

    double getProbability( void ) { return probability_; }
    ProbabilityStream& setProbability(double probability) { probability_ = probability; return *this; }

  protected:

    // Arrive here when the stream starts or backs up: flip a weighted
    // coin to choose whether or not to play the source stream.
    ProbabilityStream& reset() {
      double p = ((double) rand() / (RAND_MAX));
      current_stream_ = (p < probability_) ? getSource() : NULL;
      return *this;
    }

    Tick prev_tick_;
    Stream * current_stream_;
    double probability_;
  
  };                              // class ProbabilityStream

} // namespace mu

#endif
