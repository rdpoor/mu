/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/
#include "file_read_stream.h"

namespace mu {

  FileReadStream::FileReadStream()
    : file_name_ ( "" ),
      do_normalize_ ( true ) {
    temp_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  FileReadStream::~FileReadStream() {
  }
  
  void FileReadStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getFileName() = " << getFileName() << std::endl;
  }

  void FileReadStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    if (!file_read_.isOpen()) {
      file_read_.open(file_name_);
    }
    Tick s = std::max((long)0, (long)tick);
    Tick e = std::min((long)(file_read_.fileSize()), (long)(tick+buffer.frames()));
    Tick n_frames = e - s;
    if (n_frames <= 0) {
      // no overlap at all
      zeroBuffer(buffer);
    } else if (n_frames == buffer.frames()) {
      // file spans whole buffer
      file_read_.read(buffer, s, do_normalize_);
    } else {
      // partial overlap.  Read into a temp buffer and copy
      // the active part into buffer.
      temp_buffer_.resize(n_frames, buffer.channels());
      file_read_.read(temp_buffer_, s, do_normalize_);
      copyBuffer(temp_buffer_, 0, buffer, s - tick, n_frames);
    }
  }

  Tick FileReadStream::getStart() { 
    return 0; 
  }

  Tick FileReadStream::getEnd() { 
    if (!file_read_.isOpen()) {
      // TODO: how best to handle errors, e.g. nonexistent file?
      file_read_.open(file_name_);
    }
    return (file_read_.fileSize()); 
  }

  FileReadStream& FileReadStream::fileName(std::string file_name) {
    file_name_ = file_name; return *this;
  }

  FileReadStream& FileReadStream::doNormalize(bool do_normalize) {
    do_normalize_ = do_normalize; return *this;
  }

  std::string FileReadStream::getFileName() const { 
    return file_name_; 
  }



}
