#include "transport.h"

namespace mu {

  // Callback function for RtAudio when requesting more data for playback.
  // NOTE: see tarballs/stk-4.4.4/src/RtWvOut.cpp
  int dac_callback( void *outputBuffer, 
                    void *inputBuffer, 
                    unsigned int nBufferFrames,
                    double streamTime, 
                    RtAudioStreamStatus status, 
                    void *dataPointer ) {
    return ( (Transport *) dataPointer )->Transport::readBuffer( outputBuffer, nBufferFrames, streamTime );
  }
  
  // Callback method for RtAudio.  buffer is an array of StkFloat
  // values, interleaved (if multichannel).  frame_count is the number
  // of sample frames.
  // 
  // Note that this is called in a separate thread, so care must be
  // taken in reading or writing any state here.
  //
  // Implementation note: RtAudio expects to receive samples in an
  // array of doubles.  Node objects expect to be called with an
  // StkFrames object.  The main work in this method is to copy data
  // out of the StkFrames object into RtAudio's buffer object.
  //
  // See also:
  // ~/Projects/Mune/tarballs/stk-4.4.4/src/RtWvOut.cpp Line::36
  //
  int Transport::readBuffer( void *buffer, unsigned int frame_count, double stream_time ) {
    TRACE(".");
    if (source_ == NULL) { return 0; };

    // grow the stkFrames buffer as needed
    stk_frames_.resize(frame_count, channel_count_);

    MuTime time = stream_time;

    // ask the source to generate samples
    source_->step(stk_frames_, time, *this);

    // copy the samples to RtAudio's buffer
    stk::StkFloat *input = (stk::StkFloat *)&stk_frames_[0];
    stk::StkFloat *output = (stk::StkFloat *)buffer;
    unsigned int sample_count = frame_count * channel_count_;

    for (unsigned int i=0; i<sample_count; i++) {
      *output++ = *input++;
    }

    return 0;
  }
  
  Transport::~Transport() {
    TRACE("Transport::~Transport()\n");
  }
  
  // Start the transport if not already running.  Does not rewind
  // before starting.
  Transport& Transport::start() {
    TRACE("Transport::start()\n");
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
                       sample_rate_,
                       &frame_size_,
                       &dac_callback,
                       (void *)this);

      dac_.startStream();
      is_running_ = true;
    }
    return *this;
  }

  
  // Stop the transport if not already stopped. If \c immediately is
  // true, immediately stops the transport, otherwise gives time for
  // already queued samples to finish.
  Transport& Transport::stop(bool immediately) {
    TRACE("Transport::stop()\n");
    if (is_running_) {
      // dac_.isStreamRunning() ?
      dac_.closeStream();
      is_running_ = false;
    }
    return *this;
  }
  
}
