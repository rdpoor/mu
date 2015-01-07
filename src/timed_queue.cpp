#include "timed_queue.h"
#include <stdio.h>
#include <algorithm>

namespace mu {

  bool timedEventComparison(TimedEvent *e0, TimedEvent *e1) {
    return e0->getTime() > e1->getTime();
  }

  void TimedQueue::enqueue(TimedEvent *event) {
    std::vector<TimedEvent *>::iterator low;
    mutex_.lock();
    low = std::lower_bound(v_.begin(), v_.end(), event, timedEventComparison);
    v_.insert(low, event);
    mutex_.unlock();
  }
};
