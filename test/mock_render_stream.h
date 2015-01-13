#include <Stk.h>
#include "render_stream.h"
#include "mu_types.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

class MockRenderStream : public mu::RenderStream {
 public:
  MOCK_METHOD4(render, void(stk::StkFrames&, mu::MuTick base_tick, mu::MuTick start_tick, mu::MuTick end_tick));
};
