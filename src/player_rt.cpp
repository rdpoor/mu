/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  

   ================================================================ 
*/

#include "player_rt.h"

namespace mu {

  // Callback function for RtAudio when requesting more data for playback.
  // NOTE: see tarballs/stk-4.4.4/src/RtWvOut.cpp
  int dac_callback( void *outputBuffer, 
                    void *inputBuffer, 
                    unsigned int nBufferFrames,
                    double streamTime, 
                    RtAudioStreamStatus status, 
                    void *dataPointer ) {
    return ((PlayerRt *)dataPointer )->PlayerRt::processBuffer(outputBuffer,
                                                               nBufferFrames,
                                                               streamTime);
  }
  

  PlayerRt::PlayerRt() : 
    device_number_ (PlayerRt::default_device_number()) {
    stk_frames_.resize(PlayerRt::default_frame_size(), 
                       PlayerRt::default_channel_count());
  }

  PlayerRt::~PlayerRt() {
  }

  void PlayerRt::start() {
    RtAudio::StreamParameters dac_parameters;
    if ( device_number_ == 0 ) {
      dac_parameters.deviceId = dac_.getDefaultOutputDevice();
    } else {
      dac_parameters.deviceId = device_number_ - 1;
    }
    dac_parameters.nChannels = channel_count_;

    if (!dac_.isStreamOpen()) {
      dac_.openStream(&dac_parameters,
                      NULL,
                      ((sizeof(MuFloat) == 8) ? 
                       RTAUDIO_FLOAT64 : 
                       RTAUDIO_FLOAT32),
                      frame_rate_,
                      &frame_size_,
                      &dac_callback,
                      (void *)this);
    }
    if (!dac_.isStreamRunning()) {
      dac_.startStream();
    }
  }

  void PlayerRt::stop() {
    if (dac_.isStreamOpen()) {
      dac_.closeStream();
    }
  }


  // Called when RtAudio DAC wants data.  +buffer+ is an array of StkFloat
  // values, interleaved if multichannel.  frame_count is the number of sample
  // frames.
  //
  // The main contract of this method is to ask the transport to fill up an
  // StkFrames object with samples and then to copy the samples into the RtAudio
  // buffer.
  //
  PlayerRt::RtAudioDirective PlayerRt::processBuffer(void *buffer, 
                                                     unsigned int frame_count, 
                                                     double stream_time) {
    if (transport_ == NULL) { 
      // this should not normally happen.
      return kStopImmediately; 
    };

    // grow the stkFrames buffer as needed
    stk_frames_.resize(frame_count, channel_count_);

    // ask the source to generate samples
    transport_->render(&stk_frames_);

    // Copy the samples to RtAudio's buffer.
    // TODO: ASSERT(sizeof(MuFloat) == sizeof(stk::StkFloat))
    MuFloat *src = (MuFloat *)&stk_frames_[0];
    MuFloat *dst = (MuFloat *)buffer;
    unsigned int sample_count = frame_count * channel_count_;
    // TODO: memcpy!
    for (int i=sample_count-1; i>=0; i--) { *dst++ = *src++; }

    // If the transport has stopped, allow the dac playback to 
    // finish up before stopping the stream.
    return (transport_->state() == Transport::kStopped) ? 
      kStopAndDrain : 
      kContinue;
  }
  
}                               // namespace mu
