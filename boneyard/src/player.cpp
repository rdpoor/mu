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
#include "player.h"

namespace mu {

  Player& Player::init() {
    setChannelCount(kDefaultChannelCount);
    setFrameRate(kDefaultFrameRate);
    setFrameSize(kDefaultFrameSize);
    setSource(NULL);
    setTick(0);
    return *this;
  }
  
  unsigned int Player::getChannelCount() const { return channel_count_; };
  Player& Player::setChannelCount(unsigned int channel_count) {
    channel_count_ = channel_count; return *this;
  }

  stk::StkFloat Player::getFrameRate() const { return frame_rate_; }
  Player& Player::setFrameRate(stk::StkFloat frame_rate) {
    frame_rate_ = frame_rate; return *this;
  }
  
  unsigned int Player::getFrameSize() const { return frame_size_; }
  Player& Player::setFrameSize(unsigned int frame_size) {
    frame_size_ = frame_size; return *this;
  }
  
  SampleProcessor *Player::getSource() const { return source_; }
  Player& Player::setSource(SampleProcessor *source) {
    source_ = source; return *this;
  }

  Tick Player::getTick() const { return tick_; }
  Player& Player::setTick(Tick tick) {
    tick_ = tick; return *this;
  }

}
