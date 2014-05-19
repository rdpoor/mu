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

    Stream *getSource() const;
    Player& setSource(Stream *source);

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
    Stream *source_;
    Tick tick_;

  };                            // class Player

} // namespace mu
#endif
