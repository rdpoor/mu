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
  }

  FileReadStream::~FileReadStream() {
  }
  
  void FileReadStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getFileName() = " << getFileName() << std::endl;
  }

  void FileReadStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    // fprintf(stderr,"FileReadStream::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    // fprintf(stderr,"FRS: A %s ", file_name_.c_str());
    if (!file_read_.isOpen()) {
      // fprintf(stderr,"FRS: a ");
      file_read_.open(file_name_);
      // TODO: check format, rate, etc against buffer & player
    }
    // fprintf(stderr,"FRS: B ");
    Tick start_frame = tick;
    Tick frames_in_file = file_read_.fileSize() - start_frame;
    Tick frames_in_buffer = buffer.frames();
    unsigned int channel_count = buffer.channels();
    // fprintf(stderr,"FRS: C ");
//     printf("buf=%p, data=%p, frames()=%d, channels=%d\n", 
//            &buffer, 
//            &(buffer[0]), 
//            buffer.frames(), 
//            buffer.channels());
//     printf("sf=%ld fif=%ld fib=%ld, cc=%d\n", 
//            start_frame, 
//            frames_in_file, 
//            frames_in_buffer, 
//            channel_count);

    if (frames_in_file > frames_in_buffer) {
      // file.read() will fill entire buffer
      // fprintf(stderr,"FRS: D fif=%ld, fib=%ld sf=%ld bf=%d ", frames_in_file, frames_in_buffer, start_frame, buffer.frames());
      file_read_.read(buffer, start_frame, do_normalize_);
      // fprintf(stderr,"d ");
    } else if (frames_in_file > 0) {
      // file.read() will partially fill the buffer
      // fprintf(stderr,"FRS: E ");
      file_read_.read(buffer, start_frame, do_normalize_);
      bzero(&(buffer[frames_in_file]), 
            (frames_in_buffer - frames_in_file) * channel_count * sizeof(stk::StkFloat));
    } else {
      // start_frame is past end of file -- zero the entire buffer
      // fprintf(stderr,"FRS: F ");
      bzero(&(buffer[0]), frames_in_buffer * channel_count * sizeof(stk::StkFloat));
    }

    // fprintf(stderr,"FRS: G\n");
    // fprintf(stderr, "step: %p %f %p\r", &buffer, time, &player);
  }

  Tick FileReadStream::getStart() { return 0; }

  Tick FileReadStream::getEnd() { 
    if (!file_read_.isOpen()) {
      // TODO: how best to handle errors, e.g. nonexistent file?
      file_read_.open(file_name_);
    }
    return (file_read_.fileSize()); 
  }

  FileReadStream& FileReadStream::fileName(std::string file_name) {
    TRACE("FileReadStream::fileName()\n");
    file_name_ = file_name; return *this;
  }

  FileReadStream& FileReadStream::doNormalize(bool do_normalize) {
    TRACE("FileReadStream::doNormalize()\n");
    do_normalize_ = do_normalize; return *this;
  }

  std::string FileReadStream::getFileName() const { 
    return file_name_; 
  }



}
