#include "rt_player.h"

namespace mu {

  // Callback function for RtAudio when requesting more data for playback.
  // NOTE: see tarballs/stk-4.4.4/src/RtWvOut.cpp
  int dac_callback( void *outputBuffer, 
                    void *inputBuffer, 
                    unsigned int nBufferFrames,
                    double streamTime, 
                    RtAudioStreamStatus status, 
                    void *dataPointer ) {
    return ( (RtPlayer *) dataPointer )->RtPlayer::readBuffer( outputBuffer, nBufferFrames, streamTime );
  }
  
  // Callback method for RtAudio.  buffer is an array of StkFloat
  // values, interleaved (if multichannel).  frame_count is the number
  // of sample frames.
  // 
  // Note that this is called in a separate thread, so care must be
  // taken in reading or writing any state here.
  //
  // Implementation note: RtAudio expects to receive samples in an
  // array of doubles.  Stream objects expect to be called with an
  // StkFrames object.  The main work in this method is to copy data
  // out of the StkFrames object into RtAudio's buffer object.
  //
  // See also:
  // ~/Projects/Mune/tarballs/stk-4.4.4/src/RtWvOut.cpp Line::36
  //
  int RtPlayer::readBuffer( void *buffer, unsigned int frame_count, double stream_time ) {
    TRACE(".");
    if (source_ == NULL) { return 0; };

    // grow the stkFrames buffer as needed
    stk_frames_.resize(frame_count, channel_count_);

    // ask the source to generate samples
    source_->step(stk_frames_, tick_, *this);

    // update tick counter
    tick_ += frame_count;

    // copy the samples to RtAudio's buffer
    stk::StkFloat *src = (stk::StkFloat *)&stk_frames_[0];
    stk::StkFloat *dst = (stk::StkFloat *)buffer;
    unsigned int sample_count = frame_count * channel_count_;
    for (unsigned int i=sample_count-1; i>=0; i--) { *dst++ = *src++; }

    return 0;
  }
  
  RtPlayer::~RtPlayer() {
    TRACE("RtPlayer::~RtPlayer()\n");
  }
  
  // Start the player if not already running.  Does not rewind
  // before starting.
  RtPlayer& RtPlayer::start() {
    TRACE("RtPlayer::start()\n");
    if (!is_running_) {
      RtAudio::StreamParameters dac_parameters;
      if ( device_number_ == 0 ) {
        dac_parameters.deviceId = dac_.getDefaultOutputDevice();
      } else {
        dac_parameters.deviceId = device_number_ - 1;
      }
      dac_parameters.nChannels = channel_count_;

      dac_.openStream( &dac_parameters,
                       NULL,
                       ((sizeof(stk::StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32),
                       frame_rate_,
                       &frame_size_,
                       &dac_callback,
                       (void *)this);

      dac_.startStream();
      is_running_ = true;
    }
    return *this;
  }

  
  // Stop the player if not already stopped. If \c immediately is
  // true, immediately stops the player, otherwise gives time for
  // already queued samples to finish.
  RtPlayer& RtPlayer::stop(bool immediately) {
    TRACE("RtPlayer::stop()\n");
    if (is_running_) {
      // dac_.isStreamRunning() ?
      dac_.closeStream();
      is_running_ = false;
    }
    return *this;
  }
  
}
