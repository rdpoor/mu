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
#include "file_write_stream.h"

namespace mu {

  FileWriteStream::FileWriteStream()
    : file_name_ ( "" ),
      file_type_ ( default_file_type() ),
      file_format_ ( default_file_format() ) {
  }

  FileWriteStream::~FileWriteStream() {
    // printf("~FileWriteStream()\n");
  }
  
  FileWriteStream *FileWriteStream::clone() {
    FileWriteStream *c = new FileWriteStream();
    c->set_file_name(file_name());
    c->set_file_type(file_type());
    c->set_file_format(file_format());
    return c;
  }

  bool FileWriteStream::render(MuTick buffer_start, MuBuffer *buffer) {
    bool any_rendered = false;

    if ((source_ != NULL) && source_->render(buffer_start, buffer)) {
      if (!file_write_.isOpen()) {
        printf("opening file %s\n", file_name_.c_str());
        file_write_.open(file_name_, buffer->channels(), file_type_, file_format_);
      }
      file_write_.write(*buffer);
      any_rendered = true;

    } else if (file_write_.isOpen()) {
      printf("closing file %s\n", file_name_.c_str());
      file_write_.close();
    }

    return any_rendered;
  }

  void FileWriteStream::inspect_aux(int level, std::stringstream* ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "file_name() = " << file_name() << std::endl;
    *ss << "file_type() = " << file_type() << std::endl;
    *ss << "file_format() = " << file_format() << std::endl;
    SingleSourceStream::inspect_aux(level, ss);
  }

} // namespace mu
