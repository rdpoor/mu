#include "single_source_rs.h"
#include "mu_types.h"
#include <Stk.h>

namespace mu {

  // Note: it is an error to set crop_start > crop_end (results are undefined).

  class CropRS : public SingleSourceRS {
  public:
    CropRS();
    ~CropRS();

    // starting frame (inclusive)
    MuTick crop_start() { return crop_start_; }
    void set_crop_start(MuTick crop_start) { crop_start_ = crop_start; }

    // ending frame (exclusive)
    MuTick crop_end() { return crop_end_; }
    void set_crop_end(MuTick crop_end) { crop_end_ = crop_end; }

    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);

  protected:
    MuTick crop_start_;
    MuTick crop_end_;
  };

}

// Local Variables:
// mode: c++
// End:
