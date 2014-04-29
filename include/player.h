#ifndef MU_PLAYER_H
#define MU_PLAYER_H

#include "mu.h"
#include <math.h>

namespace mu {

  class Player {
  public:
    static const unsigned int kDefaultChannelCount = 2u;
    // TODO: kDefaultSampleRate not currently used
    static const unsigned int kDefaultSampleRate = 44100u;
    static const unsigned int kDefaultFrameSize = 512u;

    Player& init();

    // setters and getters
    unsigned int getChannelCount();
    Player& setChannelCount(unsigned int channel_count);

    stk::StkFloat getSampleRate();
    Player& setSampleRate(stk::StkFloat sample_rate);

    unsigned int getFrameSize();
    Player& setFrameSize(unsigned int frame_size);

    Node *getSource();
    Player& setSource(Node *source);

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
    stk::StkFloat sample_rate_;
    unsigned int frame_size_;
    Node *source_;
    MuTime time_;

  };                            // class Player

  inline Player& Player::init() {
    setChannelCount(kDefaultChannelCount);
    setSampleRate(kDefaultSampleRate);
    setFrameSize(kDefaultFrameSize);
    setSource(NULL);
    setTime(0.0);
    return *this;
  }

  inline unsigned int Player::getChannelCount() { return channel_count_; };
  inline Player& Player::setChannelCount(unsigned int channel_count) {
    TRACE("Player::getChannelCount()\n");
    channel_count_ = channel_count; return *this;
  }

  inline stk::StkFloat Player::getSampleRate() { return sample_rate_; }
  inline Player& Player::setSampleRate(stk::StkFloat sample_rate) {
    TRACE("Player::getSampleRate()\n");
    sample_rate_ = sample_rate; return *this;
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

  inline MuTime Player::getTime() { return time_; }
  inline Player& Player::setTime(MuTime time) {
    TRACE("Player::getTime()\n");
    time_ = time; return *this;
  }

} // namespace mu
#endif
