// a non-sound example to demonstrate MuScheduler

#include "mu.h"
#include <string>
#include <iostream>

// ================================================================

class EventStream {
public:
  EventStream() : time_(0) {}

  void schedule_event(mu::MuScheduler *scheduler) {
    printf("schedule_event = %p\n", scheduler);

    scheduler->schedule_event(time_, [&](mu::MuScheduler *) {
	perform_event(scheduler);
      });
  }

  void perform_event(mu::MuScheduler *scheduler) {
    printf("perform_event = %p\n", scheduler);
    printf("Ping at time = %ld\n", time_);
    time_ += 1;
    if (time_ < 5) {
      schedule_event(scheduler);
    }
  }

protected:
  mu::MuTick time_;
};


// ================================================================
    
int main() {
  mu::MuScheduler scheduler;
  EventStream event_stream;

  printf("scheduler = %p\n", &scheduler);
  
  event_stream.schedule_event(&scheduler);

  while (scheduler.event_count() > 0) {
    scheduler.step();
  }
  
  return 0;
}
