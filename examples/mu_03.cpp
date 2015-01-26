// Play a sound file

#include "mu_types.h"
#include "file_read_stream.h"
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

  file_read_stream->set_file_name(EXAMPLE_DIRECTORY "purple.wav");
  transport.set_source(file_read_stream);
  transport.set_player(&player_rt);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
