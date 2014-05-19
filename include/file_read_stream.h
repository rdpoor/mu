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

    std::string getClassName() { return "FileReadStream"; }
    void inspectAux(std::stringstream& ss, int level);

    FileReadStream& step(stk::StkFrames& buffer, 
                     Tick tick,
                     Player &player);
    Tick getStart( void );
    Tick getEnd( void );

    FileReadStream& fileName(std::string file_name);
    FileReadStream& doNormalize(bool do_normalize);

    std::string getFileName() const;

  protected:
    std::string file_name_;
    bool do_normalize_;

    stk::FileRead file_read_;

  };                            // class FileReadStream

}                               // namespace mu

#endif
