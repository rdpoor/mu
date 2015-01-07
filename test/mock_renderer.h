#include "renderer.h"
#include "gmock/gmock.h"  // Brings in Google Mock.

class MockRenderer : public mu::Renderer {
 public:
  MOCK_METHOD1(Render, bool(stk::StkFrames&));
};
