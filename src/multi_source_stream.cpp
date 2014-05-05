#include "multi_source_stream.h"

namespace mu {

  Tick MultiSourceStream::getStart() {
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

  Tick MultiSourceStream::getEnd() {
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

  MultiSourceStream& MultiSourceStream::addSource(Stream *source) {
    sources_.push_back(source);
    return *this;
  }
  
  MultiSourceStream& MultiSourceStream::removeSource(Stream *source) {
    for (int i=0; i<sources_.size(); i++) {
      if (sources_.at(i) == source) {
        sources_.erase(sources_.begin()+i);
        return *this;
      }
    }
    return *this;
  }
  
  MultiSourceStream& MultiSourceStream::removeAllSources() {
    sources_.clear();
    return *this;
  }
  
  
}
