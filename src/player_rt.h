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

// File: player_rt.h
// Defines an audio player that uses the RtAudio package for platform
// independent realtime playback.

#ifndef MU_PLAYER_RT_H
#define MU_PLAYER_RT_H

#include "RtAudio.h"
#include "Stk.h"
#include "mu_types.h"
#include "player.h"

namespace mu {

  class PlayerRt : public Player {

  public:

    PlayerRt( void );
    ~PlayerRt( void );

    static const int default_device_number() {
      return 0;
    }

    int device_number() { 
      return device_number_;
    }

    void set_device_number(int device_number) {
      device_number_ = device_number;
    }

    // these need to be marked override.
    void start();
    void stop();

    enum RtAudioDirective { 
      kContinue = 0, 
      kStopAndDrain = 1, 
      kStopImmediately = 2 };
    
    // callback method for RtAudio
    RtAudioDirective processBuffer(void *buffer, 
                                     unsigned int frame_count, 
                                     double stream_time);
    
  protected:
    int device_number_;
    RtAudio dac_;
    stk::StkFrames stk_frames_;
    
  };                            // class PlayerRt

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
