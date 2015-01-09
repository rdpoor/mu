/*
 * Test out stream with scheduler bolted on.  Don't know what to call
 * it yet.  The basic idea is that the stream renders upto the time of
 * the next event in the scheduler, then evaluates the event, then
 * continues.  The event itself might stop the Transport, or modify
 * the stream by adding or deleting an element, or change a parameter
 * or print debugging info or, well, anything, including scheduling
 * another event.
 */

#include "delay_rs.h"
#include "file_read_rs.h"
#include "mu_types.h"
#include "player_rt.h"
#include "render_stream.h"
#include "timed_event.h"
#include "transport.h"
#include <algorithm>
#include <mutex>
#include <stdlib.h>
#include <vector>

namespace mu {

  // ================================================================
  // TimedEvent

  typedef void *Whazzit;        // dunno what this is supposed to be yet

  class TimedEvent {
  public:
    static TimedEvent *create(MuTime time, Whazzit form) {
      // TODO: allocate from a resource pool, since events
      // will be created and freed rapidly and often.
      return new TimedEvent(time, form);
    }

    static void dispose(TimedEvent *te) {
      delete te;
    }

    MuTime getTime() const {
      return time_;
    }

    Whazzit getForm() {
      return form_;
    }

    void call() {
      // TODO: who you gonna call?
      // form_.call();
    }

  protected:
    TimedEvent(MuTime time, Whazzit form) : time_(time), form_(form) {}
    ~TimedEvent() {}
    MuTime time_;
    Whazzit form_;

  };

  // ================================================================
  // TimedQueue

  class TimedQueue {
  public:
    TimedQueue() {
    }
    ~TimedQueue() {
    }

    TimedEvent *peekNext() {
      TimedEvent *te = NULL;
      mutex_.lock();
      if (v_.size() > 0) {
        te = v_.back();
      }
      mutex_.unlock();
      return te;
    }

    TimedEvent *getNext() {
      TimedEvent *te = NULL;
      mutex_.lock();
      if (v_.size() > 0) { 
        te = v_.back();
        v_.pop_back();
      }
      mutex_.unlock();
      return te;
    }

    // store an event in the queue for deferred evaluation.
    void enqueue(TimedEvent *event);

    // Process one event in the queue.  Returns false if there were no
    // events to process.
    bool step();

    size_t size() {
      return v_.size();
    }

    void clear() {
      mutex_.lock();
      v_.clear();
      mutex_.unlock();
    }

  protected:
    std::vector<TimedEvent *> v_;
    std::mutex mutex_;
  };

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
  
  // pop from end, evaluate event, free event
  bool TimedQueue::step() {
    TimedEvent *te = getNext();
    if (te != NULL) {
      te->call();
      delete te;
      return true;
    } else {
      return false;
    }
  }

  // ================================================================
  
}

// ================================================================
//
int main() {

}
