#include "file_write_stream.h"

namespace mu {

  FileWriteStream::FileWriteStream()
    : file_name_ ( "" ),
      file_type_ ( stk::FileWrite::FILE_WAV ),
      file_format_ ( stk::Stk::STK_SINT16 ) {
  }

  FileWriteStream::~FileWriteStream() {
    TRACE("FileWriteStream::~FileWriteStream()\n");
  }
  
  void FileWriteStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getFileName() = " << getFileName() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }


  FileWriteStream& FileWriteStream::step(stk::StkFrames& buffer, 
                                         Tick tick,
                                         Player& player) {
    
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      source_->step(buffer, tick, player);
    }

    Tick buf_s = tick;
    Tick buf_e = tick + buffer.frames();
   
    if (((getStart() == kIndefinite) || (getStart() < buf_e)) &&
        ((getEnd() == kIndefinite) || (getEnd() >= buf_s))) {
      if (!file_write_.isOpen()) {
        file_write_.open(file_name_, buffer.channels(), file_type_, file_format_);
      }
      file_write_.write(buffer);
    } else {
      file_write_.close();
    }

    return *this;
  }

} // namespace mu
