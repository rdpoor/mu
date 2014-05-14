#include "crop_stream.h"
#include <strings.h>

namespace mu {

  CropStream::~CropStream() {
    TRACE("CropStream::~CropStream()\n");
  }

  // TODO: put this in SingleSourceStream
  void CropStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Input" << std::endl;
    ss << source_->inspect(level+1);
  }

  CropStream& CropStream::step(stk::StkFrames& buffer,
                             Tick tick,
                             Player& player) {
    if (source_ == NULL) {
      zero_buffer(buffer);
    } else {
      Tick buffer_start = tick;
      Tick buffer_end = tick + buffer.frames();
      Tick source_start = getStart();
      Tick source_end = getEnd();
      source_start = (source_start == kIndefinite) ? buffer_start : std::max(source_start, buffer_start);
      source_end = (source_end == kIndefinite) ? buffer_end : std::min(source_end, buffer_end);

      if ((source_start >= buffer_end) || (source_end < buffer_start)) {
        // source does not intersect with buffer.  Just zero the buffer.
        zero_buffer(buffer);

      } else if ((source_start == buffer_start) && (source_end == buffer_end)) {
        // no cropping required: let source write buffer directly
        source_->step(buffer, tick, player);

      } else {
        // gotta work harder...
        Tick frame_count = source_end - source_start;
        Tick frame_offset = source_start - buffer_start;

        // Prep buffer_ to receive samples
        buffer_.resize(frame_count, buffer.channels());
        source_->step(buffer_, source_start, player);

        // copy buffer_[0] into buffer[frame_offset] for frame_count
        // frames.  To make sure unwritten parts of the buffer are
        // cleared, zero the entire buffer first.
        zero_buffer(buffer);
        copy_buffer(buffer_, 0, buffer, frame_offset, frame_count);
      }
    }
    return *this;
  }

  // If both source_->getStart() and start_ are defined, then getStart() is
  // the larger of source_->getStart() and start_.  Otherwise, a concrete
  // value trumps an indefinite value.
  Tick CropStream::getStart() {
    if (source_ == NULL) {
      return start_;
    } else if (source_->getStart() == kIndefinite) {
      return start_;
    } else if (start_ == kIndefinite) {
      return source_->getStart();
    } else {
      return std::max(source_->getStart(), start_);
    }
  }

  // If both source_->getEnd() and end_ are defined, then getEnd() is the
  // smaller of source_->getEnd() and end_.  Otherwise, a concrete value
  // trumps an indefinite value.
  Tick CropStream::getEnd() {
    if (source_ == NULL) {
      return end_;
    } else if (source_->getEnd() == kIndefinite) {
      return end_;
    } else if (end_ == kIndefinite) {
      return source_->getEnd();
    } else {
      return std::min(source_->getEnd(), end_);
    }
  }

}
