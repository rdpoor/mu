/*
 * SequenceStream accepts an arbitrary number of source streams,
 * assigns a time offset to each stream, and sums the results
 * together.  At present, SequenceStream does not crop the input
 * streams, so overlap is permitted.
 */
#ifndef MU_SEQUENCE_STREAM_H
#define MU_SEQUENCE_STREAM_H

#include "mu.h"
#include "add_stream.h"

namespace mu {

  class SequenceStream : public AddStream {
  public:

    SequenceStream();
    ~SequenceStream( void );
    // SequenceStream& step(stk::StkFrames& buffer, Tick tick, Player &player);
    SequenceStream& addSource(Stream *source, Tick delay);

  };                            // class SequenceStream

}                               // namespace mu

#endif
