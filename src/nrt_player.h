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

#ifndef MU_NRT_PLAYER_H
#define MU_NRT_PLAYER_H

#include "mu.h"
#include "stream.h"
#include "player.h"
// #include <mutex.h>

namespace mu {

  class NrtPlayer : public Player {
  public:

    NrtPlayer();
    ~NrtPlayer(void);

    // Start the player if not already running.  Does not rewind
    // before starting.
    NrtPlayer& start();

    // Stop the player if not already stopped. If \c immediately is
    // true, immediately stops the player, otherwise gives time for
    // already queued samples to finish.
    NrtPlayer& stop(bool immediately = false);

    // Method called from processing thread
    void *processingLoop();

    bool isRunning() const { return is_running_; }

  protected:
    bool is_running_enabled_;
    bool is_running_;

    pthread_t processing_thread_;
    pthread_t current_processing_thread_;
    pthread_t main_processing_thread_;
    stk::StkFrames stk_frames_;

  };                            // class NrtPlayer

} // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
