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

#ifndef MU_FILE_READ_STREAM_H
#define MU_FILE_READ_STREAM_H

#include "mu_stream.h"
#include "mu_types.h"
#include "sound_library.h"
#include <FileRead.h>

namespace mu {

  class FileReadStream : public MuStream {
  public:

    typedef std::unordered_map<std::string, stk::FileRead *> SoundLibrary;

    // Return a FileRead object, possibly cached.
    // TODO: could be private
    static stk::FileRead *lookup( std::string file_name );

    FileReadStream();
    virtual ~FileReadStream( void );
    virtual FileReadStream *clone();

    // Get/set the file name of the sound file.
    std::string file_name(void) { return file_name_; }
    void set_file_name(std::string file_name) { 
      file_name_ = file_name;
      file_read_ = FileReadStream::lookup(file_name_);
    }

    // Return the number of channels in the sound file or 0 if not set.
    int channel_count( void ) {
      return (file_read_->isOpen()) ? file_read_->channels() : 0;
    }

    // Return the duration of the sound file, or kUndefined if not set.
    MuTick duration( void ) {
      return (file_read_->isOpen()) ? file_read_->fileSize() : kUndefined;
    }

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "FileReadStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    static SoundLibrary library_;

    stk::FileRead *file_read_;
    std::string file_name_;
    MuBuffer tmp_buffer_;

  private:
    bool verify_format(MuBuffer *buffer);
    bool copy_samples(MuTick lo, MuTick hi, MuTick buffer_start, MuBuffer *buffer);

  };                            // class FileReadStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
