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
#include "file_write_sp.h"

namespace mu {

  FileWriteSP::FileWriteSP()
    : file_name_ ( "" ),
      file_type_ ( stk::FileWrite::FILE_WAV ),
      file_format_ ( stk::Stk::STK_SINT16 ) {
  }

  FileWriteSP::~FileWriteSP() {
    TRACE("FileWriteSP::~FileWriteSP()\n");
  }
  
  void FileWriteSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getFileName() = " << getFileName() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }


  void FileWriteSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {
    
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      source_->step(buffer, tick, is_new_event);
    }

    Tick buf_s = tick;
    Tick buf_e = tick + buffer.frames();
   
    if ((TickUtils::isIndefinite(getStart()) || (getStart() < buf_e)) &&
        (TickUtils::isIndefinite(getEnd()) || (getEnd() >= buf_s))) {
      if (!file_write_.isOpen()) {
        file_write_.open(file_name_, buffer.channels(), file_type_, file_format_);
      }
      file_write_.write(buffer);
    } else {
      file_write_.close();
    }
  }

} // namespace mu
