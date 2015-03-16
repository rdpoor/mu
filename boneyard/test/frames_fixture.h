#include "gtest/gtest.h"
#include <Stk.h>

class FramesFixture : public testing::Test {
 public:

 protected:
  static unsigned int default_frame_count() { return 32; }
  static unsigned int default_channel_count() { return 2; }
  static mu::MuFloat guard_value() { return 654321.0; }

  virtual void SetUp() {
    frames_.resize(default_frame_count(), default_channel_count());
    // fill the stk::StkFrames object with a guard value.
    for (unsigned int i=0; i<frames_.frames(); i++) {
      for (unsigned int j=0; j<frames_.channels(); j++) {
        frames_(i,j) = guard_value();
      }
    }
  }

  stk::StkFrames frames_;
};

// Local Variables:
// mode: c++
// End:
