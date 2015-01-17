#include "mu_types.h"
#include "render_stream.h"
#include <Stk.h>
#include <FileRead.h>

namespace mu {

  class FileReadRS : public RenderStream {
  public:
    FileReadRS();
    ~FileReadRS();

    std::string file_name(void);
    void set_file_name(std::string file_name);

    bool render(stk::StkFrames &frames, MuTick base_tick, MuTick start_tick, MuTick end_tick);

  protected:
    bool verify_format(stk::StkFrames &frames);
    std::string file_name_;
    stk::StkFrames temp_buffer_;
    stk::FileRead file_read_;
  };

}
