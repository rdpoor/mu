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
    stk::StkFrames stk_frames_;

  };                            // class NrtPlayer

} // namespace mu
#endif
