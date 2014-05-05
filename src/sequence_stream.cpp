#include "sequence_stream.h"
#include "delay_stream.h"

namespace mu {

  SequenceStream::SequenceStream() {
    TRACE("SequenceStream::SequenceStream()\n");
  }

  SequenceStream::~SequenceStream() {
    TRACE("SequenceStream::~SequenceStream()\n");
  }

  SequenceStream& SequenceStream::addSource(Stream *source, Tick delay) {
    if (delay > 0) {
      DelayStream *delay_stream = new DelayStream();
      delay_stream->setSource(source).setDelay(delay);
      AddStream::addSource(delay_stream);
    } else {
      AddStream::addSource(source);
    }
    return *this;
  }

}
