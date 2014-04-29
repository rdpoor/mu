#ifndef MU_FILE_READER_H
#define MU_FILE_READER_H

#include "mu.h"
#include "node.h"
#include <FileRead.h>

namespace mu {

  class FileReader : public Node {
  public:

    FileReader();
    ~FileReader( void );
    FileReader& fileName(std::string file_name);
    FileReader& doNormalize(bool do_normalize);

    FileReader& step(stk::StkFrames& buffer, 
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

  };                            // class FileReader

  inline FileReader::FileReader()
    : frame_index_ (0) {
    TRACE("FileReader::FileReader()\n");
  }

  inline FileReader& FileReader::fileName(std::string file_name) {
    TRACE("FileReader::fileName()\n");
    file_name_ = file_name; return *this;
  }

  inline FileReader& FileReader::doNormalize(bool do_normalize) {
    TRACE("FileReader::doNormalize()\n");
    do_normalize_ = do_normalize; return *this;
  }

  inline Tick FileReader::streamDuration() { 
    return (double)file_read_.fileSize() / file_read_.fileRate(); 
  }

  inline std::string FileReader::getFileName() const { 
    return file_name_; 
  }

}                               // namespace mu

#endif
