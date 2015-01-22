#include "file_read_stream.h"
#include "player.h"
#include "mu_utils.h"

namespace mu {

  FileReadStream::FileReadStream()
    : file_name_("") {
    tmp_buffer_.resize(Player::default_frame_size(), 
                       Player::default_channel_count());
  }

  FileReadStream::~FileReadStream() {
  }

  // Return true if there is an open sound file and its parameters
  // (data type, sample rate, number of channels) match the frames
  // object.  TODO: This would be a good candidate for a
  // "before_playing()" callback.  TODO: return a string describing
  // the mismatch (if any)
  bool FileReadStream::verify_format(MuBuffer &buffer) {
    if (!file_read_.isOpen()) {
      return false;
    } else if (file_read_.channels() != buffer.channels()) {
      return false;
    } else if (file_read_.fileRate() != buffer.dataRate()) {
      return false;
    } else {
      return true;
    }
  }

  bool FileReadStream::render(MuBuffer &buffer, MuTick buffer_start) {

    // check for format mismatch
    if (!verify_format(buffer)) return false;

    MuTick file_end = file_read_.fileSize();
    MuTick buffer_end = buffer_start + buffer.frames();

    if (buffer_start >= file_end) {
      // Nothing to render
      return false;

    } else if (buffer_end <= file_end) {
      // Render directly into buffer
      file_read_.read(buffer, buffer_start);
      return true;

    } else {
      // render partial buffer
      tmp_buffer_.resize(buffer_end - file_end, buffer.channels());
      file_read_.read(tmp_buffer_, buffer_start);

      MuUtils::zero_buffer(buffer);
      MuUtils::copy_buffer(tmp_buffer_, buffer);
      return true;
    }
  }

} // namespace mu

