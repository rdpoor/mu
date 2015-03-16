#ifndef MU_RENDER_STREAM_H
#define MU_RENDER_STREAM_H

#include "mu_types.h"
#include <Stk.h>

namespace mu {

  class RenderStream {
  public:

    // Caller is asking target to compute samples between start_tick
    // and end_tick and to write resulting the samples into
    //   frames_[start_tick - base_tick] (inclusive) to 
    //   frames_[end_tick - base_tick] (exclusive).
    // Target promises not to touch any sample outside of that range,
    // but MAY return false to indicate that no samples were written.
    virtual bool render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) = 0;
    // Compute an index into the frame based on base_tick, tick.
    static long int frame_index(MuTick base_tick, MuTick tick) { return tick - base_tick; }

    void zero_buffer(stk::StkFrames &buffer) {
      long int n_frames = buffer.frames();
      int n_channels = buffer.channels();
      for (long int frame = n_frames - 1; frame >= 0; frame--) {
        for (int channel = n_channels - 1; channel >= 0; channel--) {
          buffer(frame, channel) = 0;
        }
      }
    }
      
  };

}

#endif

// Local Variables:
// mode: c++
// End:
