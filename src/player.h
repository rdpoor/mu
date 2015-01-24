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

// File: player.h
// Player defines a virtual real-time playback device.  Its primary contract is
// to request buffers of data from a Transport object to be played.

#ifndef MU_PLAYER_H
#define MU_PLAYER_H

#include "mu_types.h"
#include "transport.h"

namespace mu {

  class Transport;              // resolve circular dependency

  // Should you be disallowing copying?
  //
  // http://stackoverflow.com/questions/6077143/disable-copy-constructor
  class Player {
  public:
    Player();
    ~Player();

    // defaults and getters and setters

    static unsigned int default_channel_count() {
      return 2u;
    }

    unsigned int channel_count() const {
      return channel_count_;
    }

    void set_channel_count(unsigned int channel_count) {
      channel_count_ = channel_count;
    }

    static MuFloat default_frame_rate() {
      return 44100.0;
    }

    MuFloat frame_rate() const {
      return frame_rate_;
    }

    void set_frame_rate(MuFloat frame_rate) {
      frame_rate_ = frame_rate;
    }

    static unsigned int default_frame_size() {
      return 512u;
    }

    unsigned int frame_size() const {
      return frame_size_;
    }

    void set_frame_size(unsigned int frame_size) {
      frame_size_ = frame_size;
    }

    Transport *transport() const {
      return transport_;
    }
    
    void set_transport(Transport *transport) {
    transport_ = transport;
    }

    // Start the player if not already running.  Starts passing
    // buffers to Transport::render() in a dedicated thread.
    virtual void start() = 0;

    // Stop the player if not already stopped.
    virtual void stop() = 0;

  protected:
    unsigned int channel_count_;
    MuFloat frame_rate_;
    unsigned int frame_size_;
    Transport *transport_;
  };                            // class Player

}                               // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
