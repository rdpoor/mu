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
  mu::MuUtils::zero_buffer(&buffer_);         // method being tested

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
  mu::MuUtils::fill_buffer(&buffer_, expected); // method being tested
  
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
  mu::MuUtils::copy_buffer(&src_, &buffer_);   // method being tested

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
  mu::MuUtils::copy_buffer_subset(&src_, &buffer_, offset, count);

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

TEST_F(MuUtils, PitchToRatioP) {
  ASSERT_DOUBLE_EQ(2.0, mu::MuUtils::pitch_to_ratio(12.0));
}
TEST_F(MuUtils, PitchToRatioZ) {
  ASSERT_DOUBLE_EQ(1.0, mu::MuUtils::pitch_to_ratio(0.0));
}
TEST_F(MuUtils, PitchToRatioM) {
  ASSERT_DOUBLE_EQ(0.5, mu::MuUtils::pitch_to_ratio(-12.0));
}
TEST_F(MuUtils, RatioToPitchP) {
  ASSERT_DOUBLE_EQ(12.0, mu::MuUtils::ratio_to_pitch(2.0));
}
TEST_F(MuUtils, RatioToPitchZ) {
  ASSERT_DOUBLE_EQ(0.0, mu::MuUtils::ratio_to_pitch(1.0));
}
TEST_F(MuUtils, RatioToPitchM) {
  ASSERT_DOUBLE_EQ(-12.0, mu::MuUtils::ratio_to_pitch(0.5));
}

TEST_F(MuUtils, DBToRatioP) {
  ASSERT_DOUBLE_EQ(10.0, mu::MuUtils::db_to_ratio(20.0));
}
TEST_F(MuUtils, DBToRatioZ) {
  ASSERT_DOUBLE_EQ(1.0, mu::MuUtils::db_to_ratio(0.0));
}
TEST_F(MuUtils, DBToRatioM) {
  ASSERT_DOUBLE_EQ(0.1, mu::MuUtils::db_to_ratio(-20.0));
}
TEST_F(MuUtils, RatioToDBP) {
  ASSERT_DOUBLE_EQ(20.0, mu::MuUtils::ratio_to_db(10.0));
}
TEST_F(MuUtils, RatioToDBZ) {
  ASSERT_DOUBLE_EQ(0.0, mu::MuUtils::ratio_to_db(1.0));
}
TEST_F(MuUtils, RatioToDBM) {
  ASSERT_DOUBLE_EQ(-20.0, mu::MuUtils::ratio_to_db(0.1));
}
TEST_F(MuUtils, MidiPitchToFrequency) {
  ASSERT_DOUBLE_EQ(220.0, mu::MuUtils::midi_pitch_to_frequency(69.0 - 12.0));
}
TEST_F(MuUtils, FrequencyToMidiPitch) {
  ASSERT_DOUBLE_EQ(69.0 + 12.0, mu::MuUtils::frequency_to_midi_pitch(440.0 * 2.0));
}
  
