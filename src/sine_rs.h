#ifndef MU_SINE_RS_H
#define MU_SINE_RS_H

#include "render_stream.h"

namespace mu {
  class SineRS : public RenderStream {
  public:
    
    static const MuFloat default_frequency() {
      return 440.0;
    }
    
    static const MuFloat default_amplitude() {
      return 0.5;
    }
    
    static const MuFloat default_phase() {
      return 0.0;
    }
    
    SineRS()
      : amplitude_(SineRS::default_amplitude()),
        frequency_(SineRS::default_frequency()),
        phase_(SineRS::default_phase()) {}
    
    double amplitude() { return amplitude_; }
    void set_amplitude(double amplitude) { amplitude_ = amplitude; }

    double frequency() { return frequency_; }
    void set_frequency(double frequency) { frequency_ = frequency; }

    double phase() { return phase_; }
    void set_phase(double phase) { phase_ = phase; }

    bool render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);

  protected:
    double amplitude_;          // normally 1.0 max
    double frequency_;          // hz
    double phase_;              // radians
  };
    
}

#endif

// Local Variables:
// mode: c++
// End:
