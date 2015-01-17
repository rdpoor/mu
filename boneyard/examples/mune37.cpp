/*
 * Test new transport functions: create a RenderStream that
 * plays a sound file, read from kbd to start / stop / pause
 * stream.
 */

#include "mu_types.h"
#include "transport.h"
#include "player_rt.h"
#include <FileRead.h>
#include <ncurses.h>

namespace mu {

  class FileReadRS : public RenderStream {
  public:
    FileReadRS();
    ~FileReadRS();
    std::string file_name(void);
    void set_file_name(std::string file_name);

    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);
  protected:
    bool verify_format(stk::StkFrames &frames);
    std::string file_name_;
    stk::StkFrames temp_buffer_;
    stk::FileRead file_read_;
  };

  FileReadRS::FileReadRS()
    : file_name_ ( "" ) {
    temp_buffer_.resize(Player2::default_frame_size(), Player2::default_channel_count());
  }

  FileReadRS::~FileReadRS() {
  }

  std::string FileReadRS::file_name() {
    return file_name_;
  }

  void FileReadRS::set_file_name(std::string file_name) { 
    file_name_ = file_name;
    file_read_.open(file_name_);
  }

  // Return true if there is an open sound file and its parameters
  // (data type, sample rate, number of channels) match the frames
  // object.  TODO: This would be a good candidate for a
  // "before_playing()" callback.  TODO: return a string describing
  // the mismatch (if any)
  bool FileReadRS::verify_format(stk::StkFrames &frames) {
    if (!file_read_.isOpen()) return false;
    if (file_read_.channels() != frames.channels()) return false;
    if (file_read_.fileRate() != frames.dataRate()) return false;
    return true;
  }

  void FileReadRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    // check for format mismatch
    if (!verify_format(frames)) return;

    // check for end of file
    if (start_tick >= file_read_.fileSize()) return;

    // fast and loose: this simply fills as much of the buffer as
    // possible on each call without honoring start_tick or end_tick.
    file_read_.read(frames, base_tick);
  }

}

int main() {
  mu::FileReadRS file_read_rs;
  mu::Transport transport;
  mu::PlayerRt player_rt;

  file_read_rs.set_file_name("/Users/r/Projects/Mu/binary/fotu.wav");
  transport.set_render_stream(&file_read_rs);
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
    case '1':
      transport.jog(-100000);
      break;
    case '2':
      transport.jog(-10000);
      break;
    case '3':
      transport.jog(-1000);
      break;
    case '4':
      transport.jog(-100);
      break;
    case '5':
      transport.jog(-10);
      break;
    case '6':
      transport.jog(10);
      break;
    case '7':
      transport.jog(100);
      break;
    case '8':
      transport.jog(1000);
      break;
    case '9':
      transport.jog(10000);
      break;
    case '0':
      transport.jog(100000);
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
