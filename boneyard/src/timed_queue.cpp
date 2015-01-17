#include "timed_queue.h"
#include <stdio.h>
#include <algorithm>

namespace mu {

  bool timedEventComparison(TimedEvent *e0, TimedEvent *e1) {
    return e0->time() > e1->time();
  }

  void TimedQueue::enqueue(TimedEvent *event) {
    std::vector<TimedEvent *>::iterator low;
    mutex_.lock();
    low = std::lower_bound(v_.begin(), v_.end(), event, timedEventComparison);
    v_.insert(low, event);
    mutex_.unlock();
  }

  // pop from end, evaluate event, free event
  bool TimedQueue::step() {
    TimedEvent *te = getNext();
    if (te != NULL) {
      te->call(this);
      return true;
    } else {
      return false;
    }
  }

};
