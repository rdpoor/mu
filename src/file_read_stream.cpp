/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ================================================================
*/

#include "file_read_stream.h"
#include "mu_utils.h"
#include "player.h"

namespace mu {

  FileReadStream::FileReadStream()
    : file_name_("") {
    tmp_buffer_.resize(Player::default_frame_size(), 
                       Player::default_channel_count());
  }

  FileReadStream::~FileReadStream() {
  }

  FileReadStream *FileReadStream::clone() {
    FileReadStream *c = new FileReadStream();
    c->set_file_name(file_name());
    return c;
  }

  // Return true if there is an open sound file and its parameters
  // (data type, sample rate, number of channels) match the frames
  // object.  TODO: This would be a good candidate for a
  // "before_playing()" callback.  TODO: return a string describing
  // the mismatch (if any)
  bool FileReadStream::verify_format(MuBuffer *buffer) {
    // nit-pick: no need for "else" (but takes less vertical space)
    if (!file_read_.isOpen()) {
      return false;
    } else if (file_read_.channels() != buffer->channels()) {
      return false;
    } else if (file_read_.fileRate() != buffer->dataRate()) {
      return false;
    } else {
      return true;
    }
  }

  bool FileReadStream::render(MuTick buffer_start, MuBuffer *buffer) {

    // check for format mismatch
    if (!verify_format(buffer)) return false;

    MuTick file_end = file_read_.fileSize();
    MuTick buffer_end = buffer_start + buffer->frames();

    if (buffer_start >= file_end) {
      // Nothing to render
      return false;

    } else if (buffer_end <= file_end) {
      // Render directly into buffer
      file_read_.read(*buffer, buffer_start);
      return true;

    } else {
      // render partial buffer
      tmp_buffer_.resize(buffer_end - file_end, buffer->channels());
      file_read_.read(tmp_buffer_, buffer_start);

      MuUtils::zero_buffer(buffer);
      MuUtils::copy_buffer(&tmp_buffer_, buffer);
      return true;
    }
  }

  void FileReadStream::inspect_aux(std::stringstream& ss, int level) {
    inspect_indent(ss, level);
    ss << "file_name() = " << file_name() << std::endl;
  }

}                               // namespace mu

