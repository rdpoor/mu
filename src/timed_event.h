#ifndef MU_TIMED_EVENT_H
#define MU_TIMED_EVENT_H

#include <stdio.h>
#include "mu_closure.h"

// Consider using functors in TimedEvent.  e.g.:
//   http://www.newty.de/fpt/functor.html

namespace mu {

  typedef double Time;
  typedef void *Whazzit;        // dunno what this is supposed to be yet

  class TimedEvent {
  public:
    static TimedEvent *create(Time time, Whazzit form) {
      // TODO: allocate from a resource pool, since events
      // will be created and freed rapidly and often.
      return new TimedEvent(time, form);
    }

    static void dispose(TimedEvent *te) {
      delete te;
    }

    Time getTime() const {
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
    TimedEvent(Time time, Whazzit form) : time_(time), form_(form) {}
    ~TimedEvent() {}
    Time time_;
    Whazzit form_;

  };

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
