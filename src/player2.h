#ifndef MU_PLAYER2_H
#define MU_PLAYER2_H

#include "mu_types.h"
#include "transport.h"

namespace mu {

  class Transport;              // forward ref

  class Player2 {
  public:
    Player2();
    ~Player2();

    // defaults and getters and setters

    static const unsigned int default_channel_count() {
      return 2u;
    }

    unsigned int channel_count() const {
      return channel_count_;
    }

    void set_channel_count(unsigned int channel_count) {
      channel_count_ = channel_count;
    }

    static const MuFloat default_frame_rate() {
      return 44100.0;
    }

    MuFloat frame_rate() const {
      return frame_rate_;
    }

    void set_frame_rate(MuFloat frame_rate) {
      frame_rate_ = frame_rate;
    }

    static const unsigned int default_frame_size() {
      return 512u;
    }

    unsigned int frame_size() const {
      return frame_size_;
    }

    void set_frame_size(unsigned int frame_size) {
      frame_size_ = frame_size;
    }

    Transport *transport() const {
      return transport_;
    }
    
    void set_transport(Transport *transport) {
    transport_ = transport;
    }

    // Start the player if not already running.  Starts passing
    // buffers to Transport::render() in a dedicated thread.
    virtual void start() = 0;

    // Stop the player if not already stopped.
    virtual void stop() = 0;

  protected:
    unsigned int channel_count_;
    MuFloat frame_rate_;
    unsigned int frame_size_;
    Transport *transport_;
  };                            // class Player

} // namespace mu
#endif

// Local Variables:
// mode: c++
// End:
