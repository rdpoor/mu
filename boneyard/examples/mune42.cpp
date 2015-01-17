/*
 * Test out scheduler
 */

#include "timed_queue.h"
#include "timed_event.h"

class Binky {
public:
  void bink(mu::TimedQueue *timed_queue, mu::TimedEvent *timed_event) {
    printf("t = %4ld bink!\n", timed_event->time());
    if (timed_event->time() < 1000) {
      // adjust the event time and re-queue it.
      timed_event->set_time(timed_event->time() + 100);
      timed_queue->enqueue(timed_event);
    } else {
      // done with this event -- free it.
      delete timed_event;
    }
  }
};

void binky_callback(mu::TimedQueue *timed_queue, mu::TimedEvent *timed_event) {
  // The callback method is responsible for freeing or re-queueing the event.
  ((Binky *)(timed_event->user_data()))->bink(timed_queue, timed_event);
}

int main() {

  Binky binky;
  mu::TimedQueue tq;
  mu::TimedEvent *te = new mu::TimedEvent(100, binky_callback, (void *)&binky);

  tq.enqueue(te);               // queue the first event
  while (tq.size() != 0) {      // run until there are no more events
    tq.step();
  }

}
