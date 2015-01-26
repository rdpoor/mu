// Play a sine tone with amplitude, frequency and phase modulation.

#include "mu_types.h"
#include "sine_stream.h"
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

int main() {
  mu::Transport transport;
  mu::PlayerRt player_rt;
  mu::SineStream *sine_stream = new mu::SineStream();
  mu::SineStream *am_stream = new mu::SineStream();
  mu::SineStream *pm_stream = new mu::SineStream();

  sine_stream->set_a0(0.5);
  sine_stream->set_f0(440.0);
  sine_stream->set_p0(0.0);

  am_stream->set_a0(0.25);
  am_stream->set_f0(0.27);

  pm_stream->set_a0(12.0);
  pm_stream->set_f0(440.1);

  sine_stream->set_am_source(am_stream);
  sine_stream->set_pm_source(pm_stream);
  transport.set_source(sine_stream);
  transport.set_player(&player_rt);

  transport.run();
  wait_for_input();
  transport.stop();

  return 0;
}
