/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  

   ================================================================ 
*/

#include "mu_utils.h"
#include "gain_stream.h"
#include <math.h>

namespace mu {

  GainStream::GainStream()
    : gain_(default_gain()),
      signal_source_(NULL),
      gain_source_(NULL) {
  }
    
  GainStream::~GainStream() {
    // printf("~GainStream()\n");
    if (signal_source_) delete signal_source_;
    if (gain_source_) delete gain_source_;
  }

  GainStream *GainStream::clone() {
    GainStream *c = new GainStream();
    c->set_gain(gain());
    if (signal_source_) c->set_signal_source(signal_source()->clone());
    if (gain_source_) c->set_gain_source(gain_source()->clone());
    return c;
  }
  
  bool GainStream::render(MuTick buffer_start, MuBuffer *buffer) {
    int n_frames = buffer->frames();
    int n_channels = buffer->channels();

    if (!signal_source_) { 
      return false;
    } 

    if (signal_source_->render(buffer_start, buffer) == false) {
      return false;
    }

    if (gain_source_) {
      // take gain from gain stream
      gain_buffer_.resize(n_frames, n_channels);
      MuUtils::zero_buffer(&gain_buffer_);
      if (gain_source_->render(buffer_start, &gain_buffer_) == false) {
        // "nothing rendered, nothing gained..."
        return false;
      }

      for (MuTick tick = n_frames-1; tick >=0 ; --tick) {
        for (int channel = n_channels-1; channel >= 0; --channel) {
          (*buffer)(tick, channel) *= gain_buffer_(tick, channel);
        }
      }

    } else {
      // take gain from constant
      for (MuTick tick = n_frames-1; tick >=0 ; --tick) {
        for (int channel = n_channels-1; channel >= 0; --channel) {
          (*buffer)(tick, channel) *= gain_;
        }
      }
    }

    return true;
  }

  void GainStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "gain() = " << gain() << std::endl;
    if (signal_source()) {
      inspect_indent(level, ss); 
      *ss << "signal_source()" << std::endl;
      signal_source()->inspect_aux(level+1, ss);
    }
    if (gain_source()) {
      inspect_indent(level, ss); 
      *ss << "gain_source()" << std::endl;
      gain_source()->inspect_aux(level+1, ss);
    }
  }


}                               // namespace mu
