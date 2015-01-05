#ifndef TIMED_EVENT_H
#define TIMED_EVENT_H

#include <stdio.h>

namespace mu {

  typedef double Time;

  class TimedEvent {
  public:
    static TimedEvent *create(Time time, void *value) {
      // TODO: allocate from a resource pool, since events
      // will be created and freed rapidly and often.
      return new TimedEvent(time, value);
    }

    Time getTime() const {
      return time_;
    }

    void *getValue() {
      return value_;
    }

    bool operator<(TimedEvent const *other) {
      bool r = this->getTime() < other->getTime();
      return r;
    }

  protected:
    TimedEvent(Time time, void *value) : time_(time), value_(value) {}
    Time time_;
    void *value_;

  };

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
