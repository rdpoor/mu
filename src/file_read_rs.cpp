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

  bool FileReadRS::render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    // check for format mismatch
    if (!verify_format(frames)) return false;

    // check for end of file
    if (start_tick >= file_read_.fileSize()) return false;

    if ((start_tick == base_tick) && (end_tick == base_tick + frames.frames())) {
      // render directly into frames
      file_read_.read(frames, base_tick);
      return true;

    } else if (start_tick < end_tick) {
      temp_buffer_.resize(end_tick - start_tick, frames.channels());
      file_read_.read(temp_buffer_, start_tick);
      // TODO: memcpy!
      MuTick offset = start_tick - base_tick;
      for (int i=0; i<temp_buffer_.frames(); i++) {
        for (int j=0; j<frames.channels(); j++) {
          frames(i+offset, j) = temp_buffer_(i, j);
        }
      }
      return true;

    } else {
      return false;             // nothing to render

    }
  }

} // namespace mu

