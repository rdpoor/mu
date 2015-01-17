/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  
   ================================================================
*/

// File: buffer_fixture.h
// Create a MuBuffer filled with a guard value for general testing.

#include "gtest/gtest.h"
#include "mu_types.h"

class BufferFixture : public testing::Test {
 public:

 protected:
  static int default_frame_count() { return 32; }
  static int default_channel_count() { return 2; }
  static mu::MuFloat guard_value() { return 654321.0; }

  virtual void SetUp() {
    buffer_.resize(default_frame_count(), default_channel_count());
    int n_channels = buffer_.channels();

    for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
      for (int channel = n_channels-1; channel >= 0; channel--) {
        buffer_(tick, channel) = guard_value();
      }
    }
  }

  mu::MuBuffer buffer_;
};

// Local Variables:
// mode: c++
// End:
