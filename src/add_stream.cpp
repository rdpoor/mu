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
    // fprintf(stderr,"AddStream::%p.step(%p, %ld, %p)\n", this, &buffer, tick, &player);
    if (sources_.size() == 0) {
      zero_buffer(buffer);
    } else {
      // first source is written directly.
      Stream* source = sources_.at(0);
      // fprintf(stderr,"[%ld]=>Source::%p.step(%p, %ld, %p)\n", 0l, source, &buffer, tick, &player);
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
          buffer_.resize(buffer.frames(), buffer.channels());
          // fprintf(stderr,"[%ld]=>Source::%p.step(%p, %ld, %p)\n", i, source, &buffer, tick, &player);
          source->step(buffer_, tick, player);
          stk::StkFloat *dst = &(buffer[0]);
          stk::StkFloat *src = &(buffer_[0]);
          for (int i=buffer.size()-1; i>=0; i--) { *dst++ += *src++; }
        }
      }
    }
    return *this;
  }

}
