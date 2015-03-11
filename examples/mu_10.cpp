// Demonstrate SequenceStream

#include "mu.h"
#include <string>
#include <iostream>

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/sounds/"

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

const mu::MuTick kTicsPerBeat = 44100/4;

mu::MuTick beat_to_tick(mu::MuFloat beat) {
  return beat * kTicsPerBeat;
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;

  mu::FileReadStream *file_read_stream = new mu::FileReadStream();
  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "thump.wav");

  mu::SequenceStream *sequence_stream = new mu::SequenceStream();

  sequence_stream->add_source(file_read_stream->clone(), 
                              beat_to_tick(0.001), 
                              mu::MuUtils::db_to_ratio(-3.0));
  sequence_stream->add_source(file_read_stream->clone(), 
                              beat_to_tick(1.0), 
                              mu::MuUtils::db_to_ratio(-3.0));
  sequence_stream->add_source(file_read_stream->clone(), 
                              beat_to_tick(2.0), 
                              mu::MuUtils::db_to_ratio(-3.0));
  sequence_stream->add_source(file_read_stream->clone(), 
                              beat_to_tick(3.0), 
                              mu::MuUtils::db_to_ratio(-3.0));
  sequence_stream->add_source(file_read_stream->clone(), 
                              beat_to_tick(3.5), 
                              mu::MuUtils::db_to_ratio(-10.0));

  mu::LoopStream *loop_stream = new mu::LoopStream();
  loop_stream->set_interval(beat_to_tick(4.0));
  loop_stream->set_source_end(beat_to_tick(4.0));
  loop_stream->set_source(sequence_stream);

  transport.set_source(loop_stream);
  transport.set_player(&player_rt);
  
  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
