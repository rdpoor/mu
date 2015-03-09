// a non-sound example to demonstrate MuScheduler

#include "mu.h"
#include <string>
#include <iostream>

// ================================================================

class EventStream {
public:
  EventStream() : time_(0) {}

  void set_scheduler(mu::MuScheduler *scheduler) {
    scheduler_ = scheduler;
  }

  void schedule_event() {
    scheduler_->schedule_event(time_, [&]() {
	perform_event();
      });
  }

  void perform_event() {
    printf("Ping at time = %ld\n", time_);
    if (time_ < 5) {
      time_ += 1;
      schedule_event();
    }
  }

protected:
  mu::MuScheduler *scheduler_;
  mu::MuTick time_;
};


// ================================================================
    
int main() {
  mu::MuScheduler scheduler;
  EventStream event_stream;

  event_stream.set_scheduler(&scheduler);
  event_stream.schedule_event();

  while (scheduler.event_count() > 0) {
    scheduler.step();
  }
  
  return 0;
}
