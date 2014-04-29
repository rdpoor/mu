#include "looper.h"

namespace mu {

  Looper::~Looper() {
    TRACE("Looper::~Looper()\n");
  }
  
  Looper& Looper::step(stk::StkFrames& buffer, 
                       MuTime time,
                       const Player& player) {
    MuTime te = time + buffer.frames() / buffer.dataRate();
    MuTime slice_start_time = time;

    while (slice_start_time < te) {
      MuTime slice_end_time = floor((slice_start_time + loop_duration_) / loop_duration_) * loop_duration_;
      MuTime slice_duration = slice_end_time - slice_start_time;
      unsigned int slice_frame_offset = (slice_start_time - time) * buffer.dataRate();
      unsigned int slice_frame_length = std::min((unsigned int)(slice_duration * buffer.dataRate()),
                                                 (unsigned int)(buffer.frames() - slice_frame_offset));
//       printf("Looper: sst=%f, sd=%f, sfo=%d, sfl=%d\n", 
//              slice_start_time,
//              slice_duration,
//              slice_frame_offset,
//              slice_frame_length);
      if ((slice_frame_offset == 0) && (slice_frame_length == buffer.frames())) {
        // optimize a common case: no copy needed
        source_->step(buffer, fmod(slice_start_time, loop_duration_), player);
      } else {
        // adjust buffer_ size to receive slice_frame_length frames
        buffer_.resize(slice_frame_length, buffer.channels());
        // fill with source's data
        source_->step(buffer_, fmod(slice_start_time, loop_duration_), player);
        // copy buffer_'s data into buffer
        unsigned int sample_count = buffer_.size();
        unsigned int slice_sample_offset = slice_frame_offset * buffer.channels();
        for (int i=0; i<sample_count; i++) { buffer[i+slice_sample_offset] = buffer[i]; }
      }
      slice_start_time += slice_duration;
    }

    return *this;
  }

}
