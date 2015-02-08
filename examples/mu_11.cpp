// Demonstrate RandomSelectStream: in a loop, play "thump" 75% of the time and
// "purple" 25% of the time.

#include "mu.h"

#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

const mu::MuTick kTicsPerBeat = 44100/16;

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  return beat * kTicsPerBeat;
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;

  mu::FileReadStream *file_read_stream_01 = new mu::FileReadStream();
  file_read_stream_01->set_file_name(EXAMPLE_DIRECTORY "thump.wav");

  mu::FileReadStream *file_read_stream_02 = new mu::FileReadStream();
  file_read_stream_02->set_file_name(EXAMPLE_DIRECTORY "purple.wav");

  mu::RandomSelectStream *random_select_stream = new mu::RandomSelectStream();
  random_select_stream->add_source(file_read_stream_01, 3.0);
  random_select_stream->add_source(file_read_stream_02, 1.0);

  mu::LoopStream *loop_stream = new mu::LoopStream();
  loop_stream->set_interval(beat_to_tick(4.0));
  loop_stream->set_source_end(beat_to_tick(4.0));
  loop_stream->set_source(random_select_stream);

  transport.set_source(loop_stream);
  transport.set_player(&player_rt);
  
  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
