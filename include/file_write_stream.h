#ifndef MU_FILE_WRITE_STREAM_H
#define MU_FILE_WRITE_STREAM_H

#include "mu.h"
#include "single_source_stream.h"
#include <Stk.h>
#include <FileWrite.h>

namespace mu {

  class FileWriteStream : public SingleSourceStream {
  public:

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
