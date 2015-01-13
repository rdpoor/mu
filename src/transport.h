#ifndef MU_TRANSPORT_H
#define MU_TRANSPORT_H

// #include "mu2.h"
#include "player2.h"
#include "render_stream.h"

namespace mu {

  class Player2;                // forward ref

  class Transport {
  public:

    Transport();
    ~Transport();

    // Get/Set the Player.  The Player's contract is to request
    // buffers of sample data by calling the Transport's Render()
    // method.
    // 
    // "It is an error" to change players when in any state other than
    // TRANSPORT_STOPPED.
    //
    // Implementation note: this getter and setter is defined NOT inline
    // to avoid problems with circular dependencies.
    Player2 *player();
    void set_player(Player2 *player);

    // Get/Set the Render_Stream.  The contract of the Render_Stream is to fill
    // a buffer of sample data on demand.  "It is an error" to change
    // clients when in any state other than TRANSPORT_STOPPED.
    RenderStream *source() { 
      return source_;
    }

    void set_source(RenderStream *source) {
      source_ = source;
    }

    enum TransportState { 
      kStopped = 0, 
      kRunning, 
      kPaused };

    TransportState state() {
      return state_;
    }

    // Start the transport if not already running.
    void run();

    // Stop the transport and release system resources if not already
    // stopped.
    void stop();

    // Stop playing without releasing system resources.
    // Implementation note: while paused, the processing thread
    // actually keeps running and passing buffers to the Transport,
    // but the transport simple zeros the buffer.
    //
    // NOTE: The delay in calling run() is so small that we could
    // probably dispense with the pause() method.
    void pause();

    // Get the current playback time in sample frames.
    MuTick tick() {
      return tick_;
    }

    // Set the current playback time.  The next rendered buffer will
    // start at this time.
    void seek(MuTick tick) {
      tick_ = tick;
    }

    // Start at zero.
    void rewind() {
      seek(0);
    }

    void jog(MuTick ticks) {
      tick_ += ticks;
    }

    // Called from Player in a dedicated processing thread.
    void render(stk::StkFrames &frames);

  protected:
    Player2 *player_;
    RenderStream *source_;
    TransportState state_;
    MuTick tick_;

  };                            // class Transport

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
