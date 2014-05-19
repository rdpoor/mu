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

    ProbabilityStream( void );
    ~ProbabilityStream( void );

    std::string getClassName() { return "ProbabilityStream"; }
  
    ProbabilityStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    Tick getStart();
    Tick getEnd();

    ProbabilityStream& setSource(Stream *source) { source_ = source; return *this; }

    double getProbability( void ) { return probability_; }
    ProbabilityStream& setProbability(double probability) { probability_ = probability; return *this; }

  protected:

    ProbabilityStream& reset();

    Tick prev_tick_;
    Stream * current_stream_;
    double probability_;
  
  };                              // class ProbabilityStream

} // namespace mu

#endif
