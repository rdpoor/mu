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
  // array of doubles.  SampleProcessor objects expect to be called with an
  // StkFrames object.  The main work in this method is to copy data
  // out of the StkFrames object into RtAudio's buffer object.
  //
  // See also:
  // ~/Projects/Mune/tarballs/stk-4.4.4/src/RtWvOut.cpp Line::36
  //
  int RtPlayer::readBuffer( void *buffer, unsigned int frame_count, double stream_time ) {
    if (source_ == NULL) { return 0; };

    // grow the stkFrames buffer as needed
    stk_frames_.resize(frame_count, channel_count_);

    // ask the source to generate samples
    source_->step(stk_frames_, tick_, is_first_);

    is_first_ = false;

    // update tick counter
    tick_ += frame_count;

    // copy the samples to RtAudio's buffer
    stk::StkFloat *src = (stk::StkFloat *)&stk_frames_[0];
    stk::StkFloat *dst = (stk::StkFloat *)buffer;
    unsigned int sample_count = frame_count * channel_count_;
    for (int i=sample_count-1; i>=0; i--) { *dst++ = *src++; }

    if ((!TickUtils::isIndefinite(source_->getEnd())) && (tick_ >= source_->getEnd())) {
      stop();
    }

    return 0;
  }
  
  RtPlayer::RtPlayer() 
  : device_number_ (kDefaultDeviceNumber),
    is_running_ (false) {
    stk_frames_.resize(stk::RT_BUFFER_SIZE, 2);
    init();
  }

  RtPlayer::~RtPlayer() {
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

      is_first_ = true;
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
      // dac_.isSPRunning() ?
      dac_.closeStream();
      is_running_ = false;
    }
    return *this;
  }
  
}
