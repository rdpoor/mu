/*
 * ValidatorStream calls a validator method at each frame with current
 * tick, an stkFrames object holding the frame, and an index into the
 * stkFrames object.  The validator should return true if valid and
 * false if not valid.
 */
#ifndef MU_VALIDATOR_STREAM_H
#define MU_VALIDATOR_STREAM_H

#include "mu.h"
#include "node.h"

typedef bool (*ValidatorCallback)( mu::Tick tick, stk::StkFrames& buffer, long int frame_index);

namespace mu {

  class ValidatorStream : public Node {
  public:

    ValidatorStream();
    ~ValidatorStream( void );

    ValidatorStream& step(stk::StkFrames& buffer, 
                          Tick tick,
                          const Player &player);
    Tick streamDuration();

    Node *getSource();
    ValidatorStream& setSource(Node *source);

    ValidatorCallback getCallback();
    ValidatorStream& setCallback(ValidatorCallback callback);

  protected:
    Node *source_;
    ValidatorCallback callback_;

  };                            // class ValidatorStream

  inline ValidatorStream::ValidatorStream() {
    TRACE("ValidatorStream::ValidatorStream()\n");
  }

  inline Node *ValidatorStream::getSource() { return source_; }
  inline ValidatorStream& ValidatorStream::setSource(Node *source) {
    TRACE("ValidatorStream::source()\n");
    source_ = source; return *this;
  }
  
  inline ValidatorCallback ValidatorStream::getCallback() { return callback_; }
  inline ValidatorStream& ValidatorStream::setCallback(ValidatorCallback callback) {
    TRACE("ValidatorStream::setCallback()\n");
    callback_ = callback; return *this;
  }

}                               // namespace mu

#endif
