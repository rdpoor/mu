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
#include "crop_sp.h"
#include <strings.h>

namespace mu {

  CropSP::CropSP()
    : start_ (TickUtils::indefinite()), 
      end_ (TickUtils::indefinite()) {
    buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  CropSP::~CropSP() {
  }

  // TODO: put this in SingleSourceSP
  void CropSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  void CropSP::step(stk::StkFrames& buffer, Tick tick, bool is_new_event) {

    // fprintf(stderr,"CropSP::step(tick=%ld)\n",tick);

    if (source_ == NULL) {
      zeroBuffer(buffer);
    } else {
      Tick buffer_start = tick;
      Tick buffer_end = tick + buffer.frames();
      Tick source_start = getStart();
      Tick source_end = getEnd();
      source_start = (TickUtils::isIndefinite(source_start)) ? buffer_start : std::max(source_start, buffer_start);
      source_end = (TickUtils::isIndefinite(source_end)) ? buffer_end : std::min(source_end, buffer_end);

      if ((source_start >= buffer_end) || (source_end < buffer_start)) {
        // source does not intersect with buffer.  Just zero the buffer.
        zeroBuffer(buffer);

      } else if ((source_start == buffer_start) && (source_end == buffer_end)) {
        // no cropping required: let source write buffer directly
        source_->step(buffer, tick, is_new_event);

      } else {
        // gotta work harder...
        Tick frame_count = source_end - source_start;
        Tick frame_offset = source_start - buffer_start;

        // Prep buffer_ to receive samples
#if 0
        buffer_.resize(frame_count, buffer.channels());
#else
        // should do this once at alloc time.  Make it so 
        // resize() never calls free().
        buffer_.resize(buffer.frames(), buffer.channels());
        buffer_.resize(frame_count, buffer.channels());
#endif
        source_->step(buffer_, source_start, is_new_event);

        // copy buffer_[0] into buffer[frame_offset] for frame_count
        // frames.  To make sure unwritten parts of the buffer are
        // cleared, zero the entire buffer first.
        zeroBuffer(buffer);
        copyBuffer(buffer_, 0, buffer, frame_offset, frame_count);
      }
    }
  }

  // If both source_->getStart() and start_ are defined, then getStart() is
  // the larger of source_->getStart() and start_.  Otherwise, a concrete
  // value trumps an indefinite value.
  Tick CropSP::getStart() {
    if (source_ == NULL) {
      return start_;
    } else if (TickUtils::isIndefinite(source_->getStart())) {
      return start_;
    } else if (TickUtils::isIndefinite(start_)) {
      return source_->getStart();
    } else {
      return std::max(source_->getStart(), start_);
    }
  }

  // If both source_->getEnd() and end_ are defined, then getEnd() is the
  // smaller of source_->getEnd() and end_.  Otherwise, a concrete value
  // trumps an indefinite value.
  Tick CropSP::getEnd() {
    if (source_ == NULL) {
      return end_;
    } else if (TickUtils::isIndefinite(source_->getEnd())) {
      return end_;
    } else if (TickUtils::isIndefinite(end_)) {
      return source_->getEnd();
    } else {
      return std::min(source_->getEnd(), end_);
    }
  }

}
