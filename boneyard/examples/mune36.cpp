/*
 * Test new transport functions: create a RenderStream that
 * plays a sine tone, read from kbd to start / stop / pause
 * stream.
 */

#include "mu_types.h"
#include "transport.h"
#include "player_rt.h"
#include <math.h>
#include <ncurses.h>

namespace mu {

#define FREQ 440.0
#define AMP 0.5

  class Sine2SP : public RenderStream {
    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);
  };
    
  void Sine2SP::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    for (MuTick i=start_tick; i<end_tick; i++) {
      double u = (double)(i) / frames.dataRate();
      double v = AMP * sin(u * 2.0 * M_PI * FREQ);
      for (int j = 0; j < frames.channels(); j++) {
        frames(i-base_tick, j) = v;
      }
    }
  }
  
}

int main() {
  mu::Sine2SP sine_sp;
  mu::Transport transport;
  mu::PlayerRt player_rt;

  transport.set_render_stream(&sine_sp);
  transport.set_player(&player_rt);

  initscr();
  clear();
  noecho();
  cbreak();	/* Line buffering disabled. pass on everything */

  bool quit = false;
  while (!quit) {
    int c = getch();
    switch (c) {
    case 'a':
      printf("stop "); fflush(stdout);
      transport.stop();
      break;
    case 's':
      printf("pause "); fflush(stdout);
      transport.pause();
      break;
    case 'd':
      printf("run "); fflush(stdout);
      transport.run();
      break;
    case 'f':
      printf("rew "); fflush(stdout);
      transport.rewind();
      break;
    default:
      printf("unrecognized char %d\n", c);
      quit = true;
    }
  }
  clrtoeol();
  refresh();
  endwin();
  return 0;
}
