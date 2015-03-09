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

// File: mu_scheduler_test.cpp
// Unit tests for MuScheduler

#include "gtest/gtest.h"
#include "mu_scheduler.h"
#include "mu_deferred_event.h"
#include "mu_types.h"

TEST(MuScheduler, Constructor) {
  mu::MuScheduler s;
  ASSERT_EQ(NULL, s.current_event());
  ASSERT_EQ(0, s.event_count());
}

TEST(MuScheduler, ScheduleEvent) {
  mu::MuScheduler s;
  s.schedule_event(0, NULL);

  ASSERT_EQ(1, s.event_count());
}
  
TEST(MuScheduler, ClearAllEvents) {
  mu::MuScheduler s;
  s.schedule_event(0, NULL);

  ASSERT_EQ(1, s.event_count());
  s.clear_all_events();
  ASSERT_EQ(0, s.event_count());
  
}
  
TEST(MuScheduler, NextEvent1) {
  mu::MuScheduler s;

  ASSERT_EQ(NULL, s.next_event());
  ASSERT_EQ(mu::kUndefined, s.next_event_time());
}
  
TEST(MuScheduler, NextEvent2) {
  mu::MuScheduler s;
  s.schedule_event(1, NULL);
  s.schedule_event(2, NULL);

  ASSERT_EQ(1, s.next_event_time());
}
  
TEST(MuScheduler, NextEvent3) {
  mu::MuScheduler s;
  s.schedule_event(2, NULL);
  s.schedule_event(1, NULL);

  ASSERT_EQ(1, s.next_event_time());
}
  
TEST(MuScheduler, Step1) {
  mu::MuScheduler s;
  ASSERT_NO_THROW(s.step());
}

TEST(MuScheduler, Step2) {
  mu::MuScheduler s;
  ASSERT_EQ(false, s.step());
}

class Stepper {
public:
  void step(mu::MuScheduler *s) {
    ASSERT_EQ(0, s->event_count());
    ASSERT_EQ((mu::MuDeferredEvent *)NULL, s->next_event());
    ASSERT_NE((mu::MuDeferredEvent *)NULL, s->current_event());
    ASSERT_EQ(1, s->current_event_time());
  }
};

TEST(MuScheduler, Step3) {
  mu::MuScheduler s;
  Stepper stepper;
  s.schedule_event(1, [&]() { stepper.step(&s); });
  ASSERT_EQ(true, s.step());
}
