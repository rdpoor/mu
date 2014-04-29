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

    Player& init();

    // setters and getters
    unsigned int getChannelCount();
    Player& setChannelCount(unsigned int channel_count);

    stk::StkFloat getFrameRate();
    Player& setFrameRate(stk::StkFloat frame_rate);

    unsigned int getFrameSize();
    Player& setFrameSize(unsigned int frame_size);

    Node *getSource();
    Player& setSource(Node *source);

    Tick getTick();
    Player& setTick(Tick tick);

    MuTime getTime();
    Player& setTime(MuTime time);

    // Start the player if not already running.  Does not rewind
    // before starting.
    virtual Player& start() = 0;

    // Stop the player if not already stopped. If \c immediately is
    // true, immediately stops the player, otherwise gives time for
    // already queued samples to finish.
    virtual Player& stop(bool immediately = false) = 0;

  protected:
    unsigned int channel_count_;
    stk::StkFloat frame_rate_;
    unsigned int frame_size_;
    Node *source_;
    Tick tick_;

  };                            // class Player

  inline Player& Player::init() {
    setChannelCount(kDefaultChannelCount);
    setFrameRate(kDefaultFrameRate);
    setFrameSize(kDefaultFrameSize);
    setSource(NULL);
    setTick(0);
    return *this;
  }

  inline unsigned int Player::getChannelCount() { return channel_count_; };
  inline Player& Player::setChannelCount(unsigned int channel_count) {
    TRACE("Player::getChannelCount()\n");
    channel_count_ = channel_count; return *this;
  }

  inline stk::StkFloat Player::getFrameRate() { return frame_rate_; }
  inline Player& Player::setFrameRate(stk::StkFloat frame_rate) {
    TRACE("Player::getFrameRate()\n");
    frame_rate_ = frame_rate; return *this;
  }
  
  inline unsigned int Player::getFrameSize() { return frame_size_; }
  inline Player& Player::setFrameSize(unsigned int frame_size) {
    TRACE("Player::getFrameSize()\n");
    frame_size_ = frame_size; return *this;
  }
  
  inline Node *Player::getSource() { return source_; }
  inline Player& Player::setSource(Node *source) {
    TRACE("Player::getSource()\n");
    source_ = source; return *this;
  }

  inline Tick Player::getTick() { return tick_; }
  inline Player& Player::setTick(Tick tick) {
    TRACE("Player::getTick()\n");
    tick_ = tick; return *this;
  }

  inline MuTime Player::getTime() { return getTick() / getFrameRate(); }
  inline Player& Player::setTime(MuTime time) {
    TRACE("Player::setTime()\n");
    setTick(time * getFrameRate()); return *this;
  }

} // namespace mu
#endif
