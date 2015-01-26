// Play a sine tone.

#include "mu_types.h"
#include "sine_stream.h"
#include "player_rt.h"
#include "transport.h"

#include <string>
#include <iostream>

void wait_for_input() {
  std::cout << "Hit return to quit: ";
  std::string s;
  std::getline(std::cin, s);
}

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::SineStream *sine_stream = new mu::SineStream();

  transport.set_source(sine_stream);
  transport.set_player(&player_rt);

  sine_stream->set_a0(0.5);
  sine_stream->set_f0(440.0);
  sine_stream->set_p0(0.0);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
