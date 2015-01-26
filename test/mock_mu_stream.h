#include "mu_stream.h"
#include "mu_types.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

class MockMuStream : public mu::MuStream {
 public:
  MOCK_METHOD0(clone, mu::MuStream *());
  MOCK_METHOD2(render, bool(mu::MuTick start_tick, mu::MuBuffer *));
};
