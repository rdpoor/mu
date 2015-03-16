#include "tnvm2.h"

#include "mu_types.h"
#include "player_rt.h"
#include "render_stream.h"
#include "transport.h"

#include <string>
#include <iostream>

#include "perc_kick_rs.h"

void wait_for_input() {
  std::string s;
  std::getline(std::cin, s);
}

int main() {
  mu::RenderStream *kick_rs = make_perc_kick_rs();
  mu::Transport transport;
  mu::PlayerRt player_rt;

  transport.set_source(kick_rs);
  transport.set_player(&player_rt);

  transport.run();
  wait_for_input();
  return 0;
}
  
