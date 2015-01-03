#ifndef MU_TRANSPORT_H
#define MU_TRANSPORT_H

namespace mu {

  typedef enum { TRANSPORT_STOPPED, 
                 TRANSPORT_RUNNING, 
                 TRANSPORT_PAUSED } TransportState;

  class Transport {
  public:

    // Get/Set the TransportPlayer.  The TransportPlayer does most of its 
    // for 
"It is an error" to change players when in
    // any state other than TRANSPORT_STOPPED.
    Player *getTransportPlayer() {
      return transport_player_;
    }

    Transport *setTransportPlayer(TransportPlayer *transport_player) {
      transport_player_ = transport_player;
      return *this;
    }

    // Get/Set the TransportClient.  The TransportClient receives
    // notifications about various changes in the Transport, including
    // change of state and change of time.  "It is an error" to change
    // clients when in any state other than TRANSPORT_STOPPED.
    TransportClient *getTransportClient() { 
      return transport_client_;
    }

    Transport *setTransportClient(TransportClient *transport_client) {
      transport_client_ = transport_client;
      return *this;
    }

    // On states:
    //
    // The transport has three states: Stopped (resources released),
    // Paused (resources allocated but not playing), and Running
    // (resources allocated and playing).  The transport also keeps
    // the notion of "current time", which is the time of the next
    // sample to be played.  
    // 
    // Switching from Paused to Running is generally very fast because
    // resources are already allocated.  (In fact, an implementation 
    // of Player may choose to implement Paused state by streaming 
    // zeroes to the DAC, in which case switching to Running simply
    // means starting to write samples.)
    //
    // Switching from Stopped to Running may take longer since
    // resources must be allocated.  
    //
    // Regardless, if there are no intervening calls to setTime(), the
    // system is specified to not drop any samples across any
    // combination of calls to run(), stop() and pause().

    // Start the transport running.  If the transport is already
    // running, calling run() has no effect.  If the transport is
    // stopped, run() allocates the necessary resources and starts the
    // transport running.  If the transport is paused, run() simply
    // continues the transport without allocating the resources.
    //
    // The next sample to be played is determined by this.getTime().
    virtual Transport *run() = 0;

    // Stop the transport and free resources.  Time will be set to the
    // the last played sample + 1 and state will be set to
    // TRANSPORT_STOPPED.  If state was TRANSPORT_STOPPED, then
    // calling stop() has no effect.
    virtual Transport *stop() = 0;

    // Pause the transport with resources allocated.  If the previous
    // state was TRANSPORT_RUNNING, playback will be stopped without
    // freeing resources.  If the state was TRANSPORT_STOPPED,
    // resources will be allocated without starting playback.
    // State will be set to TRANSPORT_PAUSED.
    virtual Transport *pause() = 0;

    // Get the current transport state.  Note that if this method
    // is called from any thread other than the Transport thread,
    // there may be a pending command that will cause the state
    // to change in the near future.
    virtual TransportState getTransportState() = 0;

    // Get the current playback time.
    virtual MuTime getTime() = 0;

    // Set the current playback time.  This causes a message to
    // be sent to the TransportClient.
    virtual Transport *setTime(MuTime t) = 0;

    // Get/Set the transport client.  When setting the client, inform
    // the client about the current time.
    TransportClient *getTransportClient() { 
      return transport_client_;
    }

  protected:
    TransportPlayer *transport_player_;
    TransportClient *transport_client_;

  };                            // class Transport

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
