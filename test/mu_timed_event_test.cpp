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

// File: mu_timed_event_test.cpp
// Unit tests for MuTimedEvent

#include "gtest/gtest.h"
#include "mu_types.h"
#include "mu_timed_event.h"

TEST(MuTimedEvent, Constructor) {
  mu::MuTimedEvent te;

  ASSERT_DOUBLE_EQ(mu::kUndefined, te.time());
  // ASSERT_EQ(NULL, te.action());
}

TEST(MuTimedEvent, NullCall) {
  mu::MuTimedEvent te;

  ASSERT_NO_THROW(te.call());
}

TEST(MuTimedEvent, SimpleCall) {
  mu::MuTimedEvent te;
  int captured = 0;

  te.set_action([&]() { captured = 1; });
  ASSERT_EQ(0, captured);
  te.call();
  ASSERT_EQ(1, captured);
}

class Bloop {
public:
  Bloop() : value_(0) {}
  int value() { return value_; }
  void set_value(int value) { value_ = value; }
protected:
  int value_;
};

TEST(MuTimedEvent, MethodCall) {
  mu::MuTimedEvent te;
  Bloop bloop;
  te.set_action([&]() { bloop.set_value(22); });
  ASSERT_EQ(0, bloop.value());
  te.call();
  ASSERT_EQ(22, bloop.value());
}
