#ifndef TIMED_QUEUE_H
#define TIMED_QUEUE_H

#include <vector>
#include <stdlib.h>
#include "timed_event.h"

namespace mu {

  class TimedQueue {
  public:
    TimedQueue() {
    }
    ~TimedQueue() {
    }

    TimedEvent *peekNext() {
      if (v_.size() == 0) { return NULL; }
      return v_.at(v_.size() - 1);
    }

    TimedEvent *getNext() {
      if (v_.size() == 0) { return NULL; }
      TimedEvent *e = v_.back();
      v_.pop_back();
      return e;
    }

    TimedQueue *enqueue(TimedEvent *event);
    /*
      v_.push_back(event);
      return this;
    }
    */

    size_t size() {
      return v_.size();
    }

    void clear() {
      v_.clear();
    }

  protected:
    std::vector<TimedEvent *> v_;
  };



} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
