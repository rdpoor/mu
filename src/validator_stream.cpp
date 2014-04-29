#include "validator_stream.h"

namespace mu {

  ValidatorStream::~ValidatorStream() {
    TRACE("ValidatorStream::~ValidatorStream()\n");
  }

  // stream duration is inherited from the source stream
  Tick ValidatorStream::streamDuration() {
    return (source_ == NULL) ? kIndefinite : source_->streamDuration();
  }

  ValidatorStream& ValidatorStream::step(stk::StkFrames& buffer,
                                         Tick tick,
                                         const Player& player) {
    if (source_ == NULL) return *this;

    // Fetch frames from source
    // TODO: think about a way to signal the end of the stream.
    source_->step(buffer, tick, player);

    if (callback_ == NULL) return *this;
    
    // Call validatorCallback on each frame
    Tick frame_in_buffer;
    Tick frame_overall = tick;

    // TODO: perhaps pass *this into callback so the callback
    // can stop the player on error, etc.
    for (frame_in_buffer=0; frame_in_buffer < buffer.frames(); frame_in_buffer++, frame_overall++) {
      callback_(frame_overall, buffer, frame_in_buffer);
    }

    return *this;
  }


}
