#include "mu_stream.h"
#include "mu_types.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

class MockMuStream : public mu::MuStream {
 public:
  MOCK_METHOD2(render, bool(mu::MuBuffer&, mu::MuTick start_tick));
};
