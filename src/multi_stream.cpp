#include "multi_stream.h"

namespace mu {

  Tick MultiStream::getStart() {
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

  Tick MultiStream::getEnd() {
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

  MultiStream& MultiStream::addSource(Stream *source) {
    sources_.push_back(source);
    return *this;
  }
  
  MultiStream& MultiStream::removeSource(Stream *source) {
    for (int i=0; i<sources_.size(); i++) {
      if (sources_.at(i) == source) {
        sources_.erase(sources_.begin()+i);
        return *this;
      }
    }
    return *this;
  }
  
  MultiStream& MultiStream::removeAllSources() {
    sources_.clear();
    return *this;
  }
  
  
}
