#include "file_read_rs.h"
#include "player2.h"

namespace mu {

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
    // TODO: honor start_tick and end_tick!
    file_read_.read(frames, base_tick);
  }

}

