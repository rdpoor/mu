#include "file_read_stream.h"

namespace mu {

  FileReadStream::~FileReadStream() {
    TRACE("FileReadStream::~FileReadStream()\n");
  }
  
  FileReadStream& FileReadStream::step(stk::StkFrames& buffer, 
                               Tick tick,
                               Player& player) {
    // fprintf(stderr,"FileReadStream::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    if (!file_read_.isOpen()) {
      file_read_.open(file_name_);
      // TODO: check format, rate, etc against buffer & player
    }
    Tick start_frame = tick;
    Tick frames_in_file = file_read_.fileSize() - start_frame;
    Tick frames_in_buffer = buffer.frames();
    unsigned int channel_count = buffer.channels();
//     printf("buf=%p, data=%p, frames()=%d, channels=%d\n", 
//            &buffer, 
//            &(buffer[0]), 
//            buffer.frames(), 
//            buffer.channels());
//     printf("sf=%ld fif=%ld fib=%ld, cc=%d\n", 
//            start_frame, 
//            frames_in_file, 
//            frames_in_buffer, 
//            channel_count);

    if (frames_in_file > frames_in_buffer) {
      // file.read() will fill entire buffer
      file_read_.read(buffer, start_frame, do_normalize_);
    } else if (frames_in_file > 0) {
      // file.read() will partially fill the buffer
      file_read_.read(buffer, start_frame, do_normalize_);
      bzero(&(buffer[frames_in_file]), 
            (frames_in_buffer - frames_in_file) * channel_count * sizeof(stk::StkFloat));
    } else {
      // start_frame is past end of file -- zero the entire buffer
      bzero(&(buffer[0]), frames_in_buffer * channel_count * sizeof(stk::StkFloat));
    }

    // fprintf(stderr, "step: %p %f %p\r", &buffer, time, &player);
    return *this;
  }

  Tick FileReadStream::getStart() { return 0; }
  Tick FileReadStream::getEnd() { 
    if (!file_read_.isOpen()) {
      // TODO: how best to handle errors, e.g. nonexistent file?
      file_read_.open(file_name_);
    }
    return (file_read_.fileSize()); 
  }


}
