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

#ifndef MU_FILE_READ_STREAM_H
#define MU_FILE_READ_STREAM_H

#include "mu_types.h"
#include "mu_stream.h"
#include <FileRead.h>

namespace mu {

  class FileReadStream : public MuStream {
  public:

    FileReadStream();
    ~FileReadStream( void );

    // Get/set the file name of the sound file.
    std::string file_name(void) { return file_name_; }
    void set_file_name(std::string file_name) { 
      file_name_ = file_name;
      file_read_.open(file_name_);
    }

    // Return the duration of the sound file, or kUndefined if not set.
    MuTick duration( void ) {
      return (file_read_.isOpen()) ? file_read_.fileSize() : kUndefined;
    }

    bool render(MuBuffer &buffer, MuTick buffer_start);

  protected:
    std::string file_name_;
    stk::FileRead file_read_;
    MuBuffer tmp_buffer_;

  private:
    bool verify_format(MuBuffer &fbuffer);

  };                            // class FileReadStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
