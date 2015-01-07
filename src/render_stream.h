#ifndef MU_RENDER_STREAM_H
#define MU_RENDER_STREAM_H

#include "mu_types.h"
#include <Stk.h>

namespace mu {

  class RenderStream {
  public:

    // Fill frames with sample data.  frames[0] corresponds to the
    // base_tick'th sample, the caller is requesting samples from
    // start_tick (inclusive) to end_tick (exclusive).
    virtual void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) = 0;

  };

}

#endif

// Local Variables:
// mode: c++
// End:
