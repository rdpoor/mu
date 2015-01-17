/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  
   ================================================================ 
*/

// File: transport.h
// Defines a tape deck style interface for stopping, starting, and rewinding the
// audio strema.

#ifndef MU_TRANSPORT_H
#define MU_TRANSPORT_H

#include "player.h"
#include "mu_stream.h"

namespace mu {

  class Player;                // resolve circular dependency

  class Transport {
  public:

    Transport();
    ~Transport();

    // Get/Set the Player.  The Player's contract is to request buffers of
    // sample data by calling the Transport's Render() method.
    // 
    // "It is an error" to change players when in any state other than
    // TRANSPORT_STOPPED.
    //
    // Implementation note: this getter and setter is defined NOT inline to
    // avoid problems with circular dependencies.
    Player *player();
    void set_player(Player *player);

    // Get/Set the Render_Stream.  The contract of the Render_Stream is to fill
    // a buffer of sample data on demand.  "It is an error" to change clients
    // when in any state other than TRANSPORT_STOPPED.
    MuStream *source() { 
      return source_;
    }

    void set_source(MuStream *source) {
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

    // Stop the transport and release system resources if not already stopped.
    void stop();

    // Stop playing without releasing system resources.  Implementation note:
    // while paused, the processing thread actually keeps running and passing
    // buffers to the Transport, but the transport simple zeros the buffer.
    //
    // NOTE: The delay in calling run() from a stopped state is so small that we
    // could probably dispense with the pause() method.
    void pause();

    // Get the current playback time in sample frames.
    MuTick tick() {
      return tick_;
    }

    // Set the current playback time.  The next rendered buffer will start at
    // this time.
    void seek(MuTick tick) {
      tick_ = tick;
    }

    // Start at zero.
    void rewind() {
      seek(0);
    }

    // Advance (or rewind) the playback time.
    void jog(MuTick ticks) {
      tick_ += ticks;
    }

    // Called from Player in a dedicated processing thread when the Player needs
    // another buffer of data.
    void render(MuBuffer &frames);

  protected:
    Player *player_;
    MuStream *source_;
    TransportState state_;
    MuTick tick_;

  };                            // class Transport

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
