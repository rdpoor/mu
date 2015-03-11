// Demonstrate ScheduledStream.  Connect a FileRead object just before it
// starts playing, disconnect immediately when it finishes.

#include "mu.h"
#include <string>
#include <iostream>

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/sounds/"

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

mu::MuFloat beats_per_minute() {
  return 161.3;
}

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  mu::MuFloat tics_per_beat = 44100 * 60.0 / beats_per_minute();
  return beat * tics_per_beat;
}

// ================================================================
class Something {
public:
  Something() {
    scheduled_stream_ = new mu::ScheduledStream();
    sum_stream_ = new mu::SumStream();
    scheduled_stream_->set_source(sum_stream_);
  }
  
  mu::ScheduledStream *scheduled_stream() { return scheduled_stream_; }
  mu::SumStream *sum_stream() { return sum_stream_; }
  
  void play_sound(mu::MuTick start,
		  std::string file_name,
		  mu::MuScheduler *scheduler) {
    mu::FileReadStream *frs = new mu::FileReadStream();
    frs->set_file_name(EXAMPLE_DIRECTORY + file_name);
    mu::MuTick duration = frs->duration();

    mu::DelayStream *ds = new mu::DelayStream();
    ds->set_source(frs);
    ds->set_delay(start);

    printf("at %ld, s = %ld, e = %ld\n",
	   scheduler->current_event_time(),
	   start,
	   start + duration);
    
    scheduler->schedule_event(start, [=](mu::MuScheduler *) {
	mu::MuScheduler *s = scheduled_stream()->scheduler();
	printf("at %ld, start note\n", s->current_event_time());
	sum_stream()->add_source(ds);
      });
    scheduler->schedule_event(start + duration, [=](mu::MuScheduler *) {
	mu::MuScheduler *s = scheduled_stream()->scheduler();
	printf("at %ld, stop note\n", s->current_event_time());
	sum_stream()->remove_source(ds);
	delete ds;
      });
  }

  void glip(mu::MuFloat beat) {
    if (beat >= 98) return;

    printf("glip(%f)\n", beat);
    mu::MuScheduler *s = scheduled_stream()->scheduler();
    mu::MuTick start = beat_to_tick(beat);
    double next_beat = beat + 1 - (beat * 0.01);

    play_sound(start, "60.wav", s);
    s->schedule_event(beat_to_tick(next_beat), [=](mu::MuScheduler *) {
    	glip(next_beat);
      });
  }
  
protected:
  mu::SumStream *sum_stream_;
  mu::ScheduledStream *scheduled_stream_;
};
  

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  Something something;

  something.glip(1);

  transport.set_source(something.scheduled_stream());
  transport.set_player(&player_rt);
  
  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
