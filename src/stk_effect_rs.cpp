#include "stk_effect_rs.h"

namespace mu {

  bool StkEffectRS::render(stk::StkFrames& buffer, MuTick base_tick, MuTick start_tick, MuTick end_tick) {
    if (source_ == NULL) return false;

    int n_frames = buffer.frames();
    int n_channels = buffer.channels();
    
    // zero the temp buffer
    buffer_.resize(n_frames, n_channels);
    for (MuTick i=0; i<n_frames; i++) {
      for (int ch=0; ch<n_channels; ch++) {
        buffer_(i, ch) = 0.0;
      }
    }
    // render source into the temp buffer
    source_->render(buffer_, base_tick, start_tick, end_tick);

    // If performance is a problem, unwind into 1, 2, and "many" 
    // channels
    for (MuTick tick=start_tick; tick<end_tick; tick++) {
      MuFloat total = 0.0;
      for (int ch=0; ch<n_channels; ch++) {
        total += buffer_(frame_index(base_tick, tick), ch);
      }
      MuFloat input_value = total / n_channels;

      // send sample to stk_effect
      this->tick(input_value);

      for (int ch=0; ch<n_channels; ch++) {
        // read resulting sample from stk_effect
        MuFloat output_value = last_out(ch);

        // blend between input_value and output_value
        buffer(frame_index(base_tick, tick), ch) = 
          ((1.0 - blend_) * buffer_(frame_index(base_tick, tick), ch)) + 
          (blend_ * output_value);
      }
    }
    return true;
  }



}
