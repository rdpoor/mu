/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ================================================================
*/
#include "piecewise_linear_stream.h"

namespace mu {

  PiecewiseLinearStream::PiecewiseLinearStream() {
  }
  
  PiecewiseLinearStream::~PiecewiseLinearStream() {
  }
  
  PiecewiseLinearStream *PiecewiseLinearStream::clone() {
    PiecewiseLinearStream *c = new PiecewiseLinearStream();
    for (MuBreakpoints::const_iterator it = breakpoints_.begin(); it != breakpoints_.end(); ++it) {
      c->add_breakpoint(it->first, it->second);
    }
    return c;
  }

  void PiecewiseLinearStream::add_breakpoint(MuTick frame, MuFloat value) {
    breakpoints_[frame] = value;
  }

  MuFloat PiecewiseLinearStream::lerp(MuTick frame) {
    if (frame <= breakpoints_.begin()->first) {
      return breakpoints_.begin()->second;

    } else if (frame >= breakpoints_.rbegin()->first) {
      return breakpoints_.rbegin()->second;

    } else {
      // frame lies within two breakpoints
      MuBreakpoints::const_iterator cursor = breakpoints_.lower_bound(frame);
      MuTick x1 = cursor->first;
      MuFloat y1 = cursor->second;
      --cursor;
      MuTick x0 = cursor->first;
      MuFloat y0 = cursor->second;

      return y0 + (frame - x0) * (y1 - y0) / (x1 - x0);
    }

  }

  bool PiecewiseLinearStream::render(MuTick buffer_start, MuBuffer *buffer) {
    int n_frames = buffer->frames();
    int n_channels = buffer->channels();
    
    if (breakpoints_.size() == 0) {
      return false;

    } else if (breakpoints_.size() == 1) {
      // constant everywhere
      MuFloat value = breakpoints_.begin()->second;
      for (int frame = n_frames-1; frame >= 0; frame--) {
        for (int channel = n_channels-1; channel >= 0; channel--) {
          (*buffer)(frame, channel) = value;
        }
      }
      return true;

    } else {
      // 2 or more breakpoints
      for (int frame = n_frames-1; frame >= 0; frame--) {
        MuFloat value = lerp(frame + buffer_start);
        for (int channel = n_channels-1; channel >= 0; channel--) {
          (*buffer)(frame, channel) = value;
        }
      }
      return true;
    }

  }

  void PiecewiseLinearStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    for (MuBreakpoints::const_iterator it = breakpoints_.begin(); it != breakpoints_.end(); ++it) {
      inspect_indent(level, ss);
      *ss << "breakpoint(" << it->first << "," << it->second << ")" << std::endl;
    }
  }


}                               // namespace mu
