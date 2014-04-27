#ifndef MU_TRANSPORT_H
#define MU_TRANSPORT_H

#include "mu.h"
#include "node.h"
// #include <mutex.h>
#include "RtAudio.h"

namespace mu {

  class Transport {
  public:
    static const unsigned int kDefaultChannelCount = 2u;
    // static const StkFloat kDefaultSampleRate = 44100.0;
    static const int kDefaultDeviceNumber = 0;
    static const unsigned int kDefaultFrameSize = 512u;

    Transport();
    ~Transport(void);
    Transport& channelCount(unsigned int channel_count);
    Transport& sampleRate(stk::StkFloat sample_rate);
    Transport& deviceNumber(int device_number);
    Transport& frameSize(unsigned int frame_size);
    Transport& source(Node *source);

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
    Node *get_source();
    Transport& set_source(Node *source);

  protected:

    unsigned int channel_count_;
    stk::StkFloat sample_rate_;
    int device_number_;
    unsigned int frame_size_;

    RtAudio dac_;
    // Mutex mutex_;
    Node *source_;
    bool is_running_;
    stk::StkFrames stk_frames_;

  };                            // class Transport

  inline Transport::Transport()
    : channel_count_ (kDefaultChannelCount),
      sample_rate_ (44100.0 /* kDefaultSampleRate */),
      device_number_ (kDefaultDeviceNumber),
      frame_size_ (kDefaultFrameSize),
      source_ (NULL),
      is_running_ (false) { 
    TRACE("Transport::Transport()\n");
  }
  
  inline Transport& Transport::channelCount(unsigned int channel_count) {
    TRACE("Transport::channelCount()\n");
    channel_count_ = channel_count; return *this;
  }

  inline Transport& Transport::sampleRate(stk::StkFloat sample_rate) {
    TRACE("Transport::sampleRate()\n");
    sample_rate_ = sample_rate; return *this;
  }
  
  inline Transport& Transport::deviceNumber(int device_number) {
    TRACE("Transport::deviceNumber()\n");
    device_number_ = device_number; return *this;
  }
  
  inline Transport& Transport::frameSize(unsigned int frame_size) {
    TRACE("Transport::frameSize()\n");
    frame_size_ = frame_size; return *this;
  }
  
  inline Transport& Transport::set_source(Node *source) {
    TRACE("Transport::source()\n");
    source_ = source; return *this;
  }
  
  // Get/set source of samples
  inline Node *Transport::get_source() { return source_; }

} // namespace mu
#endif
