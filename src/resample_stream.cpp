#include "resample_stream.h"
#include <strings.h>

namespace mu {

  ResampleStream::ResampleStream()
    : sample_source_ ( NULL ), 
      timing_source_ ( NULL ) {
    sample_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    timing_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ResampleStream::~ResampleStream() {
    TRACE("ResampleStream::~ResampleStream()\n");
  }

  void ResampleStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "SampleSource" << std::endl;
    ss << sample_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "TimingSource" << std::endl;
    ss << timing_source_->inspect(level+1);
  }

  ResampleStream& ResampleStream::step(stk::StkFrames& buffer,
                             Tick tick,
                             Player& player) {

    zeroBuffer(buffer);
    if ((sample_source_ == NULL) || (timing_source_ == NULL)) {
      return *this;
    }

    timing_buffer_.resize(buffer.frames(), buffer.channels());
    timing_source_->step(timing_buffer_, tick, player);

    double min = (double)LONG_MAX;
    double max = (double)LONG_MIN;

    // find min and max times in timing_buffer_
    for (int i=0; i<timing_buffer_.frames(); i++) {
      double v = timing_buffer_(i, 0); // channel 0 only

      min = (v < min) ? v : min;
      max = (v > max) ? v : max;
    }

    Tick min_tick = (Tick)floor(min);
    Tick max_tick = (Tick)ceil(max);
    // add 1 to for sample[i+1] interpolation.
    // TODO: do we really need to add one?
    Tick n_frames = (max_tick - min_tick) + 1;

    // Fetch source samples
    sample_buffer_.resize(n_frames, buffer.channels());
    sample_source_->step(sample_buffer_, min_tick, player);

    for (int i=0; i<timing_buffer_.frames(); i++) {
      stk::StkFloat f_index = timing_buffer_(i,0); // channel 0 only
      f_index -= min_tick;                         // reference to start of sample_buffer_
      Tick i_index = ( Tick )f_index;
      stk::StkFloat alpha = f_index - i_index;
      for (int j=0; j<buffer.channels(); j++) {
        if (alpha == 0.0) {
          buffer(i,j) = sample_buffer_(i_index, j);
        } else {
          stk::StkFloat y0 = sample_buffer_(i_index, j);
          stk::StkFloat y1 = sample_buffer_(i_index+1, j);
          stk::StkFloat dy = y1 - y0;
          buffer(i,j) = y0 + dy * alpha;
        }
      }
    }
    return *this;
  }

}
