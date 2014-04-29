#include "looper.h"

namespace mu {

  Looper::~Looper() {
    TRACE("Looper::~Looper()\n");
  }
  
  Looper& Looper::step(stk::StkFrames& buffer, 
                       Tick tick,
                       Player& player) {
    Tick te = tick + buffer.frames();
    Tick slice_start_tick = tick;

    while (slice_start_tick < te) {
      Tick slice_end_tick = ((slice_start_tick + loop_duration_) / loop_duration_) * loop_duration_;
      Tick slice_duration = slice_end_tick - slice_start_tick;
      Tick slice_frame_offset = slice_start_tick - tick;
      Tick slice_frame_length = std::min((Tick)slice_duration, (Tick)(buffer.frames() - slice_frame_offset));
//       printf("Looper: sst=%f, sd=%f, sfo=%d, sfl=%d\n", 
//              slice_start_time,
//              slice_duration,
//              slice_frame_offset,
//              slice_frame_length);
      if ((slice_frame_offset == 0) && (slice_frame_length == buffer.frames())) {
        // optimize a common case: no copy needed
        source_->step(buffer, slice_start_tick % loop_duration_, player);
      } else {
        // adjust buffer_ size to receive slice_frame_length frames
        buffer_.resize(slice_frame_length, buffer.channels());
        // fill with source's data
        source_->step(buffer_, slice_start_tick % loop_duration_, player);
        // copy buffer_'s data into buffer
        unsigned int sample_count = buffer_.size();
        unsigned int slice_sample_offset = slice_frame_offset * buffer.channels();
        for (int i=0; i<sample_count; i++) { buffer[i+slice_sample_offset] = buffer[i]; }
      }
      slice_start_tick += slice_duration;
    }

    return *this;
  }

}
