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
    TRACE("Player::getChannelCount()\n");
    channel_count_ = channel_count; return *this;
  }

  stk::StkFloat Player::getFrameRate() const { return frame_rate_; }
  Player& Player::setFrameRate(stk::StkFloat frame_rate) {
    TRACE("Player::getFrameRate()\n");
    frame_rate_ = frame_rate; return *this;
  }
  
  unsigned int Player::getFrameSize() const { return frame_size_; }
  Player& Player::setFrameSize(unsigned int frame_size) {
    TRACE("Player::getFrameSize()\n");
    frame_size_ = frame_size; return *this;
  }
  
  Stream *Player::getSource() const { return source_; }
  Player& Player::setSource(Stream *source) {
    TRACE("Player::getSource()\n");
    source_ = source; return *this;
  }

  Tick Player::getTick() const { return tick_; }
  Player& Player::setTick(Tick tick) {
    TRACE("Player::getTick()\n");
    tick_ = tick; return *this;
  }

  MuTime Player::getTime() const { return getTick() / getFrameRate(); }
  Player& Player::setTime(MuTime time) {
    TRACE("Player::setTime()\n");
    setTick(time * getFrameRate()); return *this;
  }

}
