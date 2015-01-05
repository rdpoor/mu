#include "gtest/gtest.h"

class RendererTest : public testing::Test {

#if 0
  // return true if buffer contains all zeros
  boolean allZeros(stk::StkFrames& buffer) {
    for (int i=0; i<buffer.frames(); i++) {
      for (int j=0; j<buffer.channels(); j++) {
        if (buffer(i, j) != 0) {
          return false;
        }
      }
    }
    return true;
  }
#endif
};

TEST(RendererTest, Render) {
#if 0
  Renderer renderer_;
  renderer_.setSource(NULL);
#endif
} 
