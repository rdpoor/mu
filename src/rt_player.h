/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/

#ifndef MU_RT_PLAYER_H
#define MU_RT_PLAYER_H

#include "mu.h"
#include "sample_processor.h"
#include "player.h"
// #include <mutex.h>
#include "RtAudio.h"

namespace mu {

  class RtPlayer : public Player {
  public:
    static const int kDefaultDeviceNumber = 0;

    RtPlayer( void );
    ~RtPlayer( void );

    // Start the player if not already running.  Does not rewind
    // before starting.
    RtPlayer& start();

    // Stop the player if not already stopped. If \c immediately is
    // true, immediately stops the player, otherwise gives time for
    // already queued samples to finish.
    RtPlayer& stop(bool immediately = false);

    // callback method for RtAudio
    int readBuffer( void *buffer, unsigned int frame_count, double stream_time );

    int getDeviceNumber() const { return device_number_; }
    RtPlayer& setDeviceNumber(int device_number) {
      TRACE("RtPlayer::deviceNumber()\n");
      device_number_ = device_number; return *this;
    }
    
  protected:
    int device_number_;
    RtAudio dac_;
    // Mutex mutex_;
    bool is_running_;
    stk::StkFrames stk_frames_;

  };                            // class RtPlayer

} // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
