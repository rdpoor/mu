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

#ifndef MU_FILE_WRITE_STREAM_H
#define MU_FILE_WRITE_STREAM_H

#include "mu.h"
#include "single_source_stream.h"
#include <Stk.h>
#include <FileWrite.h>

namespace mu {

  class FileWriteStream : public SingleSourceStream {
  public:

    // static const stk::FileWrite::FILE_TYPE kDefaultFileType = stk::FileWrite::FILE_WAV;
    // static const stk::FileWrite::StkFormat kDefaultFileFormat = stk::Stk::STK_SINT16;

    FileWriteStream();
    ~FileWriteStream( void );

    std::string getClassName() { return "FileWriteStream"; }
    void inspectAux(std::stringstream& ss, int level);

    FileWriteStream& step(stk::StkFrames& buffer, 
                          Tick tick,
                          Player &player);

    FileWriteStream& setSource(Stream *source) { source_ = source; return *this; }

    std::string getFileName( void ) { return file_name_; }
    FileWriteStream& setFileName(std::string file_name) { 
      file_name_ = file_name; return *this; 
    }

    stk::FileWrite::FILE_TYPE getFileType( void ) { return file_type_; }
    FileWriteStream& setFileType(stk::FileWrite::FILE_TYPE file_type) { 
      file_type_ = file_type; return *this; 
    }

    stk::FileWrite::StkFormat getFileFormat( void ) { return file_format_; }
    FileWriteStream& setFileFormat(stk::FileWrite::StkFormat file_format) {
      file_format_ = file_format; return *this;
    }

  protected:
    std::string file_name_;
    stk::FileWrite::FILE_TYPE file_type_;
    stk::Stk::StkFormat file_format_;

    stk::FileWrite file_write_;

  };                            // class FileWriteStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
