/*
 * Create classes to test streaming of audio buffers using primitives
 * in our target API: step(), reset(), seek().  Implements mu:Transport,
 * mu:Stream, mu:IdentityStream.
 */

#include "Stk.h"
#include "RtAudio.h"
#include "FileRead.h"
#include "Mutex.h"
#include <signal.h>

namespace mu {

  using namespace stk;

  // #define TRACE(s)
#define TRACE(s) fprintf(stderr, (s))

  typedef double MuTime;

  // ================================================================
  // ================================================================
  // Stream (virtual)

  // .h ================
  class Stream {
  public:
    
    const static int INDEFINITE = -1;
    
    //! Write upto nFrames of interleaved sample data into buffer.
    //! Returns number of frames written into the buffer.
    virtual Stream& step(stk::StkFrames& buffer, 
                       unsigned int frame_count, 
                       unsigned int channel_count) = 0;
    
    //! reset the Stream to time 0, reinitialize state.
    Stream& reset() {
      TRACE("Stream::reset()\n");
      releaseResources();
      acquireResources();
      seek(0.0);
      return *this;
    }

    //! allocate resources required by this stream.
    virtual Stream& acquireResources() = 0;

    //! release any resources used by this stream
    virtual Stream& releaseResources() = 0;

    //! condition the Stream so the next call to step() will fetch samples
    //! starting at \c time.
    virtual Stream& seek(MuTime time) = 0;

    //! Return the number of frames remaining in this stream, or
    //! INDEFINITE if the stream has an infinite or indeterminate
    //! number of frames.
    virtual long int framesRemaining() = 0;

  };

  // ================================================================
  // ================================================================
  // IdentityStream

  // .h ================
  class IdentityStream : public Stream {
  public:

    IdentityStream();
    ~IdentityStream( void );
    IdentityStream& step(stk::StkFrames& buffer, unsigned int frame_count, unsigned int channel_count);
    IdentityStream& acquireResources();
    IdentityStream& releaseResources();
    IdentityStream& seek(MuTime time);
    long int framesRemaining();

  protected:
    long int frame_index_;
  };                            // class IdentityStream
  
  // .cpp ================

  IdentityStream::IdentityStream() 
    : frame_index_ (0) {
    TRACE("IdentityStream::IdentityStream()\n");
  }

  IdentityStream::~IdentityStream() {
    TRACE("IdentityStream::~IdentityStream()\n");
  }

  IdentityStream& IdentityStream::step(stk::StkFrames& buffer, 
                           unsigned int frame_count, 
                           unsigned int channel_count) {
    int i = ((frame_index_ * 2) + 1) %  (frame_count * channel_count);
    TRACE("~");
    // Verify that frame_count and channel_count args match that of
    // the StackFrames buffer.
    // printf("fc=%d, cc=%d, b.fc=%d, b.cc=%d\n", 
    //        frame_count, channel_count, buffer.frames(), buffer.channels());
    // Yes, the buffer does not arrive zeroed out...
    bzero(&(buffer[0]), frame_count * channel_count * sizeof(StkFloat));
    buffer[0] = 0.9;            // exciting waveform...
    buffer[i] = 0.9;
    frame_index_ += 3;
    // fprintf(stderr, "step: %p %d %d\r", &buffer, frame_count, channel_count);
    return *this;
  }

  long int IdentityStream::framesRemaining() {
    TRACE("IdentityStream::framesRemaining()\n");
    return INDEFINITE;
  }

  IdentityStream& IdentityStream::acquireResources() {
    TRACE("IdentityStream::acquireResources()\n");
    return *this;
  }

  IdentityStream& IdentityStream::releaseResources() {
    TRACE("IdentityStream::releaseResources()\n");
    return *this;
  }

  IdentityStream& IdentityStream::seek(MuTime time) {
    TRACE("IdentityStream::seek()\n");
    return *this;
  }

  // ================
  // instance methods specific to this class
  
  // ================================================================
  // ================================================================
  // Transport is the root of the directed acyclic graph of Streams.  It
  // requests samples from streams via the step() method (called in a
  // separate thread) and passes them to the DAC for playback.
  //
  // Transport implements start, stop, seek methods to start (or
  // resume) and stop playback.  It is responsible for allocating and
  // configuring the RtAudio object that handles the real-time
  // playback.

  class Transport {

    // .h ================
  public:
    // Streaming status states.
    typedef enum { RUNNING, EMPTYING, FINISHED } TransportState;

    static const unsigned int kDefaultChannelCount = 2u;
    // static const StkFloat kDefaultSampleRate = 44100.0;
    static const int kDefaultDeviceNumber = 0;
    static const unsigned int kDefaultFrameSize = 512u;

    inline Transport()
      : channel_count_ (kDefaultChannelCount),
        sample_rate_ (44100.0 /* kDefaultSampleRate */),
        device_number_ (kDefaultDeviceNumber),
        frame_size_ (kDefaultFrameSize),
        source_ (NULL),
        is_running_ (false) { 
      TRACE("Transport::Transport()\n");
    }

    inline Transport& channelCount(unsigned int channel_count) {
      TRACE("Transport::channelCount()\n");
      channel_count_ = channel_count; return *this;
    }

    inline Transport& sampleRate(StkFloat sample_rate) {
      TRACE("Transport::sampleRate()\n");
      sample_rate_ = sample_rate; return *this;
    }

    inline Transport& deviceNumber(int device_number) {
      TRACE("Transport::deviceNumber()\n");
      device_number_ = device_number; return *this;
    }

    inline Transport& frameSize(unsigned int frame_size) {
      TRACE("Transport::frameSize()\n");
      frame_size_ = frame_size; return *this;
    }

    inline Transport& source(Stream *source) {
      TRACE("Transport::source()\n");
      source_ = source; return *this;
    }

    ~Transport(void);

    // Start the transport if not already running.  Does not rewind
    // before starting.
    Transport& start();

    // Stop the transport if not already stopped. If \c immediately is
    // true, immediately stops the transport, otherwise gives time for
    // already queued samples to finish.
    Transport& stop(bool immediately = false);

    // Seek so the next sample played will be from \c time (in score
    // time).  If the transport is currently running, the change will
    // happen after the currently queued samples have played.
    Transport& seek(MuTime time);

    // callback method
    int readBuffer( void *buffer, unsigned int frame_count );

    // Get/set source of samples
    inline Stream *source() { return source_; }

  protected:

    unsigned int channel_count_;
    StkFloat sample_rate_;
    int device_number_;
    unsigned int frame_size_;

    RtAudio dac_;
    Mutex mutex_;
    Stream *source_;
    bool is_running_;
    StkFrames stk_frames_;

  };                            // class Transport

  // .cpp ================
    
  // Callback function for RtAudio when requesting more data for playback.
  // NOTE: see tarballs/stk-4.4.4/src/RtWvOut.cpp
  int dac_callback( void *outputBuffer, 
                    void *inputBuffer, 
                    unsigned int nBufferFrames,
                    double streamTime, 
                    RtAudioStreamStatus status, 
                    void *dataPointer ) {
    return ( (Transport *) dataPointer )->Transport::readBuffer( outputBuffer, nBufferFrames );
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
  int Transport::readBuffer( void *buffer, unsigned int frame_count ) {
    TRACE(".");
    if (source_ == NULL) { return 0; };

    // grow the stkFrames buffer as needed
    stk_frames_.resize(frame_count, channel_count_);

    // ask the source to generate samples
    source_->step(stk_frames_, frame_count, channel_count_);

    // copy the samples to RtAudio's buffer
    StkFloat *input = (StkFloat *)&stk_frames_[0];
    StkFloat *output = (StkFloat *)buffer;
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
                       ((sizeof(StkFloat) == 8) ? RTAUDIO_FLOAT64 : RTAUDIO_FLOAT32),
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
  
  // Seek so the next sample played will be from \c time (in score
  // time).  If the transport is currently running, the change will
  // happen after the currently queued samples have played.
  Transport& Transport::seek(MuTime time) {
    TRACE("Transport::seek()\n");
    if (source_ != NULL) source_->seek(time);
    return *this;
  }
  

} // namespace mu

int main() {
  mu::Transport t1;
  mu::IdentityStream tn1;
  t1.source(&tn1);
  t1.start();
  sleep(5);
  t1.stop();
  return 0;
}
