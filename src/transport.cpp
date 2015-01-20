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

#include "transport.h"

namespace mu {

  Transport::Transport() 
    : player_(NULL), source_(NULL), state_(kStopped), tick_(0) {
  }

  Transport::~Transport() {
  }

  Player *Transport::player() {
    return player_;
  }
  
  void Transport::set_player(Player *player) {
    if (player_ != NULL) {
      player_->set_transport(NULL);
    }
    if (player != NULL) {
      player->set_transport(this);
    }
    player_ = player;
  }

  // Start processing thread (or ensure processing thread is running)
  // and set state to kRunning.  The player will start feeding buffers
  // via the render() method.
  void Transport::run() {
    // printf("Transport::run()"); fflush(stdout);
    if (player_ != NULL) {
      state_ = kRunning;
      player_->start();
    }
  }

  // Start the processing thread (or ensure the processing thread is
  // running) and set state to kPaused.  The player will feed buffers
  // via the render() method, but they will be ignored until the state
  // switches to kRunning.
  void Transport::pause() {
    // printf("Transport::pause()"); fflush(stdout);
    if (player_ != NULL) {
      state_ = kPaused;
      player_->start();
    }
  }

  // Stop the processing thread and set the state to kStopped.
  // Note that additional buffers may arrive in the processing
  // thread.
  void Transport::stop() {
    // printf("Transport::stop()"); fflush(stdout);
    if (player_ != NULL) {
      state_ = kStopped;
      player_->stop();
    }
  }

  void zeroFrames(MuBuffer &frames) {
    for (int i=frames.frames()-1; i>=0; i--) {
      for (int j=frames.channels()-1; j>=0; j--) {
        frames(i,j) = 0.0;
      }
    }
  }

  void Transport::render(MuBuffer &frames) {
    // printf("."); fflush(stdout);
    zeroFrames(frames);
    if ((source_ != NULL) && (state_ == kRunning)) {
      MuTick buffer_end = tick_ + frames.frames();
      if (source_->render(frames, tick_)) {
        // something got rendered
      } else {
        // nothing got rendered
      }
      tick_ = buffer_end;
    }
  }

}
