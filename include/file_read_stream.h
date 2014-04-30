#ifndef MU_FILE_READ_STREAM_H
#define MU_FILE_READ_STREAM_H

#include "mu.h"
#include "stream.h"
#include <FileRead.h>

namespace mu {

  class FileReadStream : public Stream {
  public:

    FileReadStream();
    ~FileReadStream( void );
    FileReadStream& fileName(std::string file_name);
    FileReadStream& doNormalize(bool do_normalize);

    FileReadStream& step(stk::StkFrames& buffer, 
                     Tick tick,
                     Player &player);
    Tick streamDuration();

    std::string getFileName() const;
    long int frameIndex();

  protected:
    std::string file_name_;
    bool do_normalize_;
    stk::FileRead file_read_;
    long int frame_index_;

  };                            // class FileReadStream

  inline FileReadStream::FileReadStream()
    : frame_index_ (0) {
    TRACE("FileReadStream::FileReadStream()\n");
  }

  inline FileReadStream& FileReadStream::fileName(std::string file_name) {
    TRACE("FileReadStream::fileName()\n");
    file_name_ = file_name; return *this;
  }

  inline FileReadStream& FileReadStream::doNormalize(bool do_normalize) {
    TRACE("FileReadStream::doNormalize()\n");
    do_normalize_ = do_normalize; return *this;
  }

  inline Tick FileReadStream::streamDuration() { 
    return (double)file_read_.fileSize() / file_read_.fileRate(); 
  }

  inline std::string FileReadStream::getFileName() const { 
    return file_name_; 
  }

}                               // namespace mu

#endif
