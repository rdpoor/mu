// Play a sound file with continual loop

#include "mu_types.h"
#include "crop_stream.h"
#include "file_read_stream.h"
#include "loop_stream.h"
#include "player_rt.h"
#include "transport.h"

#include <string>
#include <iostream>
#include <cmath>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

#define EXAMPLE_DIRECTORY "/Users/r/Projects/Mu/examples/"

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::FileReadStream *file_read_stream = new mu::FileReadStream();
  mu::LoopStream *loop_stream = new mu::LoopStream();

  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "purple.wav");

  mu::MuTick n_frames = file_read_stream->duration();
  loop_stream->set_source_start(0);
  loop_stream->set_source_end(n_frames);
  loop_stream->set_interval(n_frames);

  loop_stream->set_source(file_read_stream);
  transport.set_source(loop_stream);
  transport.set_player(&player_rt);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
