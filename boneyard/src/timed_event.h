#ifndef MU_TIMED_EVENT_H
#define MU_TIMED_EVENT_H

#include "mu_types.h"
#include "timed_queue.h"
#include <stdio.h>

namespace mu {

  class TimedEvent;             // forward ref
  class TimedQueue;             // forward ref

  typedef void (*TimedEventCallback)(TimedQueue *timed_queue, TimedEvent *timed_event);

  class TimedEvent {
  public:
    static TimedEvent *allocate(MuTick time, TimedEventCallback callback, void *user_data) {
      // TODO: allocate from a MuPool, since events will be created
      // and freed rapidly and often.
      return new TimedEvent(time, callback, user_data);
    }
    static void deallocate(TimedEvent *te) {
      delete te;
    }

    TimedEvent(MuTick time, TimedEventCallback callback, void *user_data) : 
      time_(time), callback_(callback), user_data_(user_data) {}
    ~TimedEvent() {}

    MuTick time() const {
      return time_;
    }
    void set_time(MuTick time) {
      time_ = time;
    }

    TimedEventCallback callback() const {
      return callback_;
    }
    void set_callback(TimedEventCallback callback) {
      callback_ = callback;
    }

    void *user_data() const {
      return user_data_;
    }
    void set_user_data(void *user_data) {
      user_data_ = user_data;
    }

    void call(TimedQueue *timed_queue) {
      callback_(timed_queue, this);
    }

  protected:
    MuTick time_;
    TimedEventCallback callback_;
    void *user_data_;
  };

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
