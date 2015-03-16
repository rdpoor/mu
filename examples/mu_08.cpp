// Mostly test out LoopStream with self-overlapping source.  Should produce
// identical results to mu_07.

#include "mu.h"
#include <string>
#include <iostream>
#include <cmath>

class Boop {
public:
  Boop() {
    sum_stream_ = new mu::SumStream();
  }

  mu::MuStream *sink() { return sum_stream_; }

  void add_boop(mu::MuTick start_time, mu::MuTick duration, mu::MuFloat frequency) {
    mu::SineStream *sine_stream = new mu::SineStream();
    sine_stream->set_a0(mu::MuUtils::db_to_ratio(-3.0));
    sine_stream->set_f0(frequency);

    mu::CropStream *crop_stream = new mu::CropStream();
    crop_stream->set_source(sine_stream);
    crop_stream->set_source_start(0);
    crop_stream->set_source_end(duration);

    mu::DelayStream *delay_stream = new mu::DelayStream();
    delay_stream->set_source(crop_stream);
    delay_stream->set_delay(start_time);

    sum_stream_->add_source(delay_stream);
  }


private:
  mu::SumStream *sum_stream_;

};                              // class Boop

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

static const mu::MuTick kTicsPerBeat = 44100/9;

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  return beat * kTicsPerBeat;
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::LoopStream *loop_stream = new mu::LoopStream();
  Boop boop;

  boop.add_boop(beat_to_tick(0), beat_to_tick(1), mu::MuUtils::midi_pitch_to_frequency(69));
  boop.add_boop(beat_to_tick(4), beat_to_tick(1.75), mu::MuUtils::midi_pitch_to_frequency(69+4));
  boop.add_boop(beat_to_tick(8), beat_to_tick(2.5), mu::MuUtils::midi_pitch_to_frequency(69+7));
  boop.add_boop(beat_to_tick(12), beat_to_tick(3.25), mu::MuUtils::midi_pitch_to_frequency(69+12));
  
  loop_stream->set_source(boop.sink());
  loop_stream->set_interval(beat_to_tick(12));
  loop_stream->set_source_end(beat_to_tick(16));

  transport.set_source(loop_stream);
  transport.set_player(&player_rt);
  
  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
