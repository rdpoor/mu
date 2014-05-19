/*
 * SineStream is a source of sine values.
 */
#ifndef MU_SINE_STREAM_H
#define MU_SINE_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class SineStream : public Stream {
  public:

    SineStream( void );
    ~SineStream( void );

    std::string getClassName() { return "SineStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    SineStream& step(stk::StkFrames& buffer, Tick tick, Player& player);

    stk::StkFloat getAmplitude() const { return amplitude_; }
    SineStream& setAmplitude(stk::StkFloat amplitude) { amplitude_ = amplitude; return *this; }

    stk::StkFloat getFrequency() const { return frequency_; }
    SineStream& setFrequency(stk::StkFloat frequency) { frequency_ = frequency; return *this; }

    stk::StkFloat getPhase() const { return phase_; }
    SineStream& setPhase(stk::StkFloat phase) { phase_ = phase; return *this; }

    stk::StkFloat getPitch() const { return 69 + 12 * log2(frequency_/440.0); }
    SineStream& setPitch(stk::StkFloat pitch) { 
      frequency_ = pow(2, (pitch-69.0)/12.0) * 440.0; 
      return *this;
    }

  protected:
    stk::StkFloat amplitude_;
    stk::StkFloat frequency_;
    stk::StkFloat phase_;
  };                            // class SineStream
}                               // namespace mu
#endif
