#include "player2.h"

namespace mu {

  Player2::Player2() {
    set_channel_count(Player2::default_channel_count());
    set_frame_rate(Player2::default_frame_rate());
    set_frame_size(Player2::default_frame_size());
    set_transport(NULL);
  }

  Player2::~Player2() {
  }

}
