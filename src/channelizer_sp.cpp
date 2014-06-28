/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/
#include "channelizer_sp.h"

namespace mu {

  ChannelizerSP::ChannelizerSP() {
    tmp_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  ChannelizerSP::~ChannelizerSP() {
  }

  void ChannelizerSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "getSourceChannelCount() = " << getSourceChannelCount() << std::endl;
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void ChannelizerSP::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else if (buffer.channels() == source_channel_count_) {
      source_->step(buffer, tick, player);
    } else {
      tmp_buffer_.resize(buffer.frames(), source_channel_count_);
      source_->step(tmp_buffer_, tick, player);

      if (source_channel_count_ == 1) {
        for (int channel = buffer.channels(); channel >= 0; channel--) {
          for (int frame = buffer.frames(); frame >= 0; frame--) {
            double sample = tmp_buffer_(frame, 0);
            buffer(frame, channel) = sample;
          }
        }
      } else if (source_channel_count_ == 2) {
        for (int channel = buffer.channels(); channel >= 0; channel--) {
          for (int frame = buffer.frames(); frame >= 0; frame--) {
            double sample = (tmp_buffer_(frame, 0) + tmp_buffer_(frame, 1)) * 0.5;
            buffer(frame, channel) = sample;
          }
        }
      } else {
        // should raise error
      }
    }

  }

}
