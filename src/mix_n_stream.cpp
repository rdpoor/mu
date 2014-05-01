#include "mix_n_stream.h"

namespace mu {

  MixNStream::MixNStream() {
    TRACE("MixNStream::MixNStream()\n");
  }

  MixNStream::~MixNStream() {
    TRACE("MixNStream::~MixNStream()\n");
  }

  MixNStream& MixNStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    if (sources_.size() == 0) {
      zero_buffer(buffer);
    } else {
      // first source is written directly.
      Stream* source = sources_.at(0);
      source->step(buffer, tick, player);
      for (long int i=1; i<sources_.size(); i++) {
        source = sources_.at(i);
        Tick buffer_start = tick;
        Tick buffer_end = tick + buffer.frames();
        Tick source_start = source->getStart();
        Tick source_end = source->getEnd();
        // TODO: write general Stream::has_overlap() method
        if (((source_start == kIndefinite) || (source_start < buffer_end)) &&
            ((source_end == kIndefinite) || (source_end > buffer_start))) {
          source->step(buffer_, tick, player);
          stk::StkFloat *dst = &(buffer[0]);
          stk::StkFloat *src = &(buffer_[0]);
          for (int i=buffer.size()-1; i>=0; i--) { *dst++ += *src++; }
        }
      }
    }
    return *this;
  }

  Tick MixNStream::getStart() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      Tick earliest_start = LONG_MAX;
      for (long int i=0; i<sources_.size(); i++) {
        Stream *source = sources_.at(i);
        Tick start = source->getStart();
        if (start == kIndefinite) {
          return kIndefinite;
        } else {
          earliest_start = std::min(earliest_start, start);
        }
      }
      return earliest_start;
    }
  }

  Tick MixNStream::getEnd() {
    if (sources_.size() == 0) {
      return kIndefinite;
    } else {
      Tick latest_end = LONG_MIN;
      for (long int i=0; i<sources_.size(); i++) {
        Stream *source = sources_.at(i);
        Tick end = source->getEnd();
        if (end == kIndefinite) {
          return kIndefinite;
        } else {
          latest_end = std::max(latest_end, end);
        }
      }
      return latest_end;
    }
  }


}
