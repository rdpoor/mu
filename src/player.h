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

#ifndef MU_PLAYER_H
#define MU_PLAYER_H

#include "mu.h"
#include <math.h>

namespace mu {

  class Player {
  public:
    static const unsigned int kDefaultChannelCount = 2u;
    static const unsigned int kDefaultFrameRate = 44100u;
    static const unsigned int kDefaultFrameSize = 512u;

    // setters and getters
    unsigned int getChannelCount() const;
    Player& setChannelCount(unsigned int channel_count);

    stk::StkFloat getFrameRate() const;
    Player& setFrameRate(stk::StkFloat frame_rate);

    unsigned int getFrameSize() const;
    Player& setFrameSize(unsigned int frame_size);

    SampleProcessor *getSource() const;
    Player& setSource(SampleProcessor *source);

    Tick getTick() const;
    Player& setTick(Tick tick);

    MuTime getTime() const;
    Player& setTime(MuTime time);

    // Start the player if not already running.  Does not rewind
    // before starting.
    virtual Player& start() = 0;

    // Stop the player if not already stopped. If \c immediately is
    // true, immediately stops the player, otherwise gives time for
    // already queued samples to finish.
    virtual Player& stop(bool immediately = false) = 0;

  protected:
    Player& init();

    unsigned int channel_count_;
    stk::StkFloat frame_rate_;
    unsigned int frame_size_;
    SampleProcessor *source_;
    Tick tick_;

  };                            // class Player

} // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
