/*
 * Test new transport functions: create a RenderStream that
 * plays a sound file, read from kbd to start / stop / pause
 * stream.  This does the same as mune37, but uses a delay
 * object rather than jogging the transport.
 */

#include "mu_types.h"
#include "transport.h"
#include "player_rt.h"
#include <FileRead.h>
#include <ncurses.h>

namespace mu {

  class DelayRS : public RenderStream {
  public:
    DelayRS();
    ~DelayRS();
    RenderStream *source() { return source_; }
    void set_source(RenderStream *source) { source_ = source; }
    MuTick delay() { return delay_; }
    void set_delay(MuTick delay) { delay_ = delay; }
    void increment_delay(MuTick increment) { delay_ += increment; }
    void render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);
  protected:
    RenderStream *source_;
    MuTick delay_;
  };

  DelayRS::DelayRS()
    : source_ ( NULL ), delay_ ( 0 ) {
  }

  DelayRS::~DelayRS() {
  }

  void DelayRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (source_ != NULL) {
      source_->render(frames, base_tick - delay_, start_tick - delay_, end_tick - delay_);
    }
  }

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
