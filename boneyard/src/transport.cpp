#include "transport.h"

namespace mu {

  Transport::Transport() 
    : player_(NULL), source_(NULL), state_(kStopped), tick_(0) {
  }

  Transport::~Transport() {
  }

  Player2 *Transport::player() {
    return player_;
  }
  
  void Transport::set_player(Player2 *player) {
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

  void zeroFrames(stk::StkFrames &frames) {
    for (int i=frames.frames()-1; i>=0; i--) {
      for (int j=frames.channels()-1; j>=0; j--) {
        frames(i,j) = 0.0;
      }
    }
  }

  void Transport::render(stk::StkFrames &frames) {
    // printf("."); fflush(stdout);
    zeroFrames(frames);
    if ((source_ != NULL) && (state_ == kRunning)) {
      MuTick end_tick = tick_ + frames.frames();
      if (source_->render(frames, tick_, tick_, end_tick)) {
        // something got rendered
      } else {
        // nothing got rendered
      }
      tick_ = end_tick;
    }
  }

}
