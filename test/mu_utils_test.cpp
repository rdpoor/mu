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

// File: mu_utils_test.cpp
// Unit tests for mu_utils.

#include "gtest/gtest.h"
#include "mu_types.h"
#include "mu_utils.h"
#include "buffer_fixture.h"

class MuUtils : public BufferFixture {
  
};

TEST_F(MuUtils, ZeroBuffer) {
  mu::MuUtils::zero_buffer(buffer_);         // method being tested

  mu::MuFloat expected = 0.0;
  int n_channels = buffer_.channels();
  for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
    for (int channel = n_channels-1; channel >= 0; channel--) {
      mu::MuFloat actual = buffer_(tick, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(MuUtils, FillBuffer) {
  mu::MuFloat expected = 1234.5;
  mu::MuUtils::fill_buffer(buffer_, expected); // method being tested
  
  int n_channels = buffer_.channels();
  for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
    for (int channel = n_channels-1; channel >= 0; channel--) {
      mu::MuFloat actual = buffer_(tick, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

class MuUtilsWithSrc : public BufferFixture {
public:

protected:
  void SetUp() {
    BufferFixture::SetUp();
    src_.resize(default_frame_count(), default_channel_count());
    int n_channels = src_.channels();

    for (mu::MuTick tick=src_.frames()-1; tick >= 0; tick--) {
      for (int channel = n_channels-1; channel >= 0; channel--) {
        src_(tick, channel) = tick;
      }
    }
  }

  mu::MuBuffer src_;
};


TEST_F(MuUtilsWithSrc, CopyBuffer) {
  mu::MuUtils::copy_buffer(src_, buffer_);   // method being tested

  int n_channels = buffer_.channels();
  for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
    mu::MuFloat expected = tick;
    for (int channel = n_channels-1; channel >= 0; channel--) {
      mu::MuFloat actual = buffer_(tick, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

TEST_F(MuUtilsWithSrc, CopyBufferSubset) {
  mu::MuTick offset = buffer_.frames() / 4.0;
  mu::MuTick count = buffer_.frames() / 2.0;
  mu::MuUtils::copy_buffer_subset(src_, buffer_, offset, count);

  int n_channels = buffer_.channels();
  for (mu::MuTick tick=buffer_.frames()-1; tick >= 0; tick--) {
    mu::MuFloat expected = guard_value();
    if ((tick >= offset) && (tick < offset + count)) {
        expected = tick - offset;
    }
    for (int channel = n_channels-1; channel >= 0; channel--) {
      mu::MuFloat actual = buffer_(tick, channel);
      ASSERT_DOUBLE_EQ(expected, actual);
    }
  }
}

