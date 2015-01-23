#include "resample_stream.h"
#include "mu_utils.h"
#include <math.h>

namespace mu {

  ResampleStream::ResampleStream()
    : sample_source_ ( NULL ), 
      timing_source_ ( NULL ) {
    sample_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    timing_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ResampleStream::~ResampleStream() {
  }

  bool ResampleStream::render(MuBuffer &buffer, MuTick buffer_start) {
    if ((sample_source_ == NULL) || (timing_source_ == NULL)) {
      return false;
    }

    timing_buffer_.resize(buffer.frames(), buffer.channels());
    if (!timing_source_->render(timing_buffer_, buffer_start)) {
      return false;
    }

    MuFloat min = (double)LONG_MAX;
    MuFloat max = (double)LONG_MIN;

    // find min and max times in timing_buffer_
    for (int i=timing_buffer_.frames()-1; i>=0; i--) {
      MuFloat v = timing_buffer_(i, 0); // channel 0 only
      min = (v < min) ? v : min;
      max = (v > max) ? v : max;
    }
    
    MuTick min_tick = (MuTick)floor(min);
    MuTick max_tick = (MuTick)ceil(max);
    // add 1 to for sample[i+1] interpolation.
    MuTick n_frames = (max_tick - min_tick) + 1;

    // Fetch source samples
    sample_buffer_.resize(n_frames, buffer.channels());
    if (!sample_source_->render(sample_buffer_, min_tick)) {
      return false;
    }

    MuUtils::zero_buffer(buffer);

    MuTick buffer_end = buffer_start + buffer.frames();

    for (MuTick tick=buffer_start; tick<buffer_end; tick++) {
      // use channel 0 (only) for timing information
      MuFloat ftau = timing_buffer_(frame_index(buffer_start, tick), 0);
      MuTick itau = ( MuTick )ftau;
      MuFloat alpha = ftau - itau;
      for (int channel = buffer.channels()-1; channel >= 0; channel--) {
        if (alpha == 0.0) {
          buffer(frame_index(buffer_start, tick), channel) = 
            sample_buffer_(frame_index(min_tick, itau), channel);
        } else {
          MuFloat y0 = sample_buffer_(frame_index(min_tick, itau), channel);
          MuFloat y1 = sample_buffer_(frame_index(min_tick, itau+1), channel);
          MuFloat dy = y1 - y0;
          buffer(frame_index(buffer_start, tick), channel) = y0 + dy * alpha;
        }
      }
    }
    return true;
  }

}
