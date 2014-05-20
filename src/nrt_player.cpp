#include "nrt_player.h"

namespace mu {

  void *process(void *thread_id) {
    return ((NrtPlayer *)thread_id)->processingLoop();
  }
  
  // processing thread.  To terminate it cleanly after processing the
  // current buffer, set current_processing_thread_ to any other
  // thread_id (e.g. that of the main thread).
  void *NrtPlayer::processingLoop() {
    current_processing_thread_ = pthread_self();
    is_running_ = true;
    while (pthread_equal(current_processing_thread_, pthread_self())) {
      if (source_ == NULL) break;
      stk_frames_.resize(frame_size_, channel_count_);
      source_->step(stk_frames_, tick_, *this);
      // TODO: protect updating of tick_ within a mutex (but right
      // now, nobody in the fg thread looks at tick_).
      tick_ += frame_size_;
      if ((source_->getEnd() != kIndefinite) && (tick_ >= source_->getEnd())) {
        stop();
      }
    }
    is_running_ = false;
    return NULL;
  }
  
  NrtPlayer::NrtPlayer()
    : is_running_enabled_ (false),
      is_running_ (false) {
    TRACE("NrtPlayer::NrtPlayer()\n");
    stk_frames_.resize(stk::RT_BUFFER_SIZE, 2); // pre-allocate stk_frames_ 
    init();
  }

  NrtPlayer::~NrtPlayer() {
    TRACE("NrtPlayer::~NrtPlayer()\n");
  }

  // Start the nrt_player if not already running.  Does not rewind
  // before starting.
  NrtPlayer& NrtPlayer::start() {
    TRACE("NrtPlayer::start()\n");
    if (!is_running_enabled_) {
      main_processing_thread_ = pthread_self();      
      int rc = pthread_create(&processing_thread_, NULL, process, (void *)this);
      if (rc) {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
      } else {
        is_running_enabled_ = true;
      }
    }
    return *this;
  }

  // Stop the nrt_player if not already stopped. If \c immediately is
  // true, immediately stops the nrt_player, otherwise gives time for
  // already queued samples to finish.
  NrtPlayer& NrtPlayer::stop(bool immediately) {
    TRACE("NrtPlayer::stop()\n");
    if (is_running_enabled_) {
      is_running_enabled_ = false;
      if (immediately) {
        // terminate with prejudice
        pthread_exit(&processing_thread_);
        is_running_ = false;
      } else {
        // modify current_processing_thread_, which processingLoop()
        // will notice on next buffer and gracefully stop.
        current_processing_thread_ = main_processing_thread_;
      }
    }
    return *this;
  }
  
}
