#include "add_stream.h"

namespace mu {

  AddStream::AddStream() {
    TRACE("AddStream::AddStream()\n");
  }

  AddStream::~AddStream() {
    TRACE("AddStream::~AddStream()\n");
  }

  void AddStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "Inputs:" << std::endl;
    for (int i=0; i<sources_.size(); i++) {
      ss << sources_.at(i)->inspect(level+1);
    }
  }
    
  AddStream& AddStream::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    int active_streams = 0;
    Tick buffer_start = tick;
    Tick buffer_end = tick + buffer.frames();

    for (int i=sources_.size()-1; i>=0; i--) {
      Stream *source = sources_.at(i);
      Tick source_start = source->getStart();
      Tick source_end = source->getEnd();
      if (((source_start == kIndefinite) || (source_start < buffer_end)) &&
          ((source_end == kIndefinite) || (source_end > buffer_start))) {
        // source has one or more frames to contribute to output.
        if (active_streams == 0) {
          // the first source can be copied directly to output.
          source->step(buffer, tick, player);
        } else {
          // subsequent sources must be fetched into a holding buffer
          // and summed into the output.
          buffer_.resize(buffer.frames(), buffer.channels());
          source->step(buffer_, tick, player);
          stk::StkFloat *dst = &(buffer[0]);
          stk::StkFloat *src = &(buffer_[0]);
          for (int j=buffer.size()-1; j>=0; j--) { *dst++ += *src++; }
        }
        active_streams += 1;
      }
    }

    // if no stream has contributed to output, explicitly zero it
    if (active_streams == 0) { zero_buffer(buffer); }
    return *this;
  }

}
