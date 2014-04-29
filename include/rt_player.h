#ifndef MU_RT_PLAYER_H
#define MU_RT_PLAYER_H

#include "mu.h"
#include "node.h"
#include "player.h"
// #include <mutex.h>
#include "RtAudio.h"

namespace mu {

  class RtPlayer : public Player {
  public:
    static const int kDefaultDeviceNumber = 0;

    RtPlayer();
    ~RtPlayer(void);

    // Start the player if not already running.  Does not rewind
    // before starting.
    RtPlayer& start();

    // Stop the player if not already stopped. If \c immediately is
    // true, immediately stops the player, otherwise gives time for
    // already queued samples to finish.
    RtPlayer& stop(bool immediately = false);

    int getDeviceNumber();
    RtPlayer& setDeviceNumber(int device_number);

    // callback method for RtAudio
    int readBuffer( void *buffer, unsigned int frame_count, double stream_time );

  protected:
    int device_number_;
    RtAudio dac_;
    // Mutex mutex_;
    bool is_running_;
    stk::StkFrames stk_frames_;

  };                            // class RtPlayer

  inline RtPlayer::RtPlayer()
    : device_number_ (kDefaultDeviceNumber),
    is_running_ (false) {
    TRACE("RtPlayer::RtPlayer()\n");
    init();
  }
  
  inline int RtPlayer::getDeviceNumber() { return device_number_; }
  inline RtPlayer& RtPlayer::setDeviceNumber(int device_number) {
    TRACE("RtPlayer::deviceNumber()\n");
    device_number_ = device_number; return *this;
  }
  
} // namespace mu
#endif
