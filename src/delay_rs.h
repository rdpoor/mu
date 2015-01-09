#include "render_stream.h"
#include "mu_types.h"
#include <Stk.h>

namespace mu {

  class DelayRS : public RenderStream {
  public:
    DelayRS();
    ~DelayRS();
    RenderStream *source() { return source_; }
    void set_source(RenderStream *source) { source_ = source; }
    MuTick delay() { return delay_; }
    void set_delay(MuTick delay) { delay_ = delay; }
    void increment_delay(MuTick increment) { delay_ += increment; }
    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);
  protected:
    RenderStream *source_;
    MuTick delay_;
  };

}
