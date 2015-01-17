/*
 * Test out the new transport and player.  Identical to mune38, but
 * now using library methods.
 *
 * Need to run this in a terminal window (not emacs) for curses lib:
 * 'a' to stop
 * 's' to pause
 * 'd' to run
 * 'f' to rewind
 * '1'..'5' backs up (even while playing)
 * '6'..'0' advances
 * Any other character quits.
 */

#include "delay_rs.h"
#include "file_read_rs.h"
#include "mu_types.h"
#include "player_rt.h"
#include "transport.h"
#include <FileRead.h>
#include <ncurses.h>

int main() {
  mu::FileReadRS file_read_rs;
  mu::DelayRS delay_rs;
  mu::Transport transport;
  mu::PlayerRt player_rt;

  file_read_rs.set_file_name("/Users/r/Projects/Mu/binary/fotu.wav");
  delay_rs.set_source(&file_read_rs);
  transport.set_render_stream(&delay_rs);
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
      delay_rs.set_delay(0);
      transport.rewind();
      break;
    case '1':
      delay_rs.increment_delay(100000);
      break;
    case '2':
      delay_rs.increment_delay(10000);
      break;
    case '3':
      delay_rs.increment_delay(1000);
      break;
    case '4':
      delay_rs.increment_delay(100);
      break;
    case '5':
      delay_rs.increment_delay(10);
      break;
    case '6':
      delay_rs.increment_delay(-10);
      break;
    case '7':
      delay_rs.increment_delay(-100);
      break;
    case '8':
      delay_rs.increment_delay(-1000);
      break;
    case '9':
      delay_rs.increment_delay(-10000);
      break;
    case '0':
      delay_rs.increment_delay(-100000);
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
