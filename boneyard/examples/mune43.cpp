/*
 * Test out stream with scheduler bolted on.  Don't know what to call
 * it yet.  The basic idea is that the stream renders upto the time of
 * the next event in the scheduler, then evaluates the event, then
 * continues.  The event itself might stop the Transport, or modify
 * the stream by adding or deleting an element, or change a parameter
 * or print debugging info or, well, anything, including scheduling
 * another event.
 */

#include "mu_types.h"
#include "player_rt.h"
#include "render_stream.h"
#include "sine_rs.h"
#include "timed_event.h"
#include "transport.h"

#include <unistd.h>

// ================================================================
//
namespace mu {

  class SchedulerRS : public RenderStream {
  public:
    SchedulerRS() : source_(NULL) {} 

    RenderStream *source() {
      return source_;
    }

    void set_source(RenderStream *source) {
      source_ = source;
    }

    TimedQueue *timed_queue() {
      return &timed_queue_;
    }

    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
      TimedEvent *next_event;
      MuTick st = start_tick;
      while (st < end_tick) {
        while (((next_event = timed_queue_.peekNext()) != NULL) && (next_event->time() <= st)) {
          timed_queue_.step();
        }
        // here next_event is null or has a start time greater than st
        MuTick et = (next_event == NULL) ? end_tick : std::min(next_event->time(), end_tick);
        if (source_ != NULL) {
          source_->render(frames, base_tick, st, et);
        }
        st = et;
      }
    }

  protected:
    TimedQueue timed_queue_;
    RenderStream *source_;

  };

}

class Swapper {
public:
  Swapper(mu::SchedulerRS *scheduler_rs) : scheduler_rs_(scheduler_rs) {}

  void set_source_a(mu::RenderStream *source) {
    source_a_ = source;
  }
  void set_source_b(mu::RenderStream *source) {
    source_b_ = source;
  }

  // When the swapper() method is called, swap source of scheduler_rs
  // between source_a and source_b, then reschedule in 0.2 seconds.
  void swapper(mu::TimedQueue *timed_queue, mu::TimedEvent *timed_event) {
    // printf("t = %4ld switch!\n", timed_event->time());
    if (scheduler_rs_->source() == source_b_) {
      scheduler_rs_->set_source(source_a_);
    } else {
      scheduler_rs_->set_source(source_b_);
    }
    // compute one sample at a time (!!)
    timed_event->set_time(timed_event->time() + 1);
    timed_queue->enqueue(timed_event);
  }
protected:
  mu::SchedulerRS *scheduler_rs_;
  mu::RenderStream *source_a_;
  mu::RenderStream *source_b_;
};

void swapper_callback(mu::TimedQueue *timed_queue, mu::TimedEvent *timed_event) {
  ((Swapper *)(timed_event->user_data()))->swapper(timed_queue, timed_event);
}

// ================================================================
//
int main() {

  mu::SineRS sine_rs_a;
  mu::SineRS sine_rs_b;
  mu::SchedulerRS scheduler_rs;
  mu::Transport transport;
  mu::PlayerRt player_rt;
  Swapper swapper(&scheduler_rs);

  mu::TimedEvent *swapper_event = new mu::TimedEvent(0, swapper_callback, (void *)&swapper);

  // schedule the swapper event
  scheduler_rs.timed_queue()->enqueue(swapper_event);

  sine_rs_a.set_frequency(880.0);
  sine_rs_b.set_frequency(660.0);

  swapper.set_source_a(&sine_rs_a);
  swapper.set_source_b(&sine_rs_b);

  transport.set_render_stream(&scheduler_rs);
  transport.set_player(&player_rt);

  transport.run();
  sleep(20);
  return 0;
}
