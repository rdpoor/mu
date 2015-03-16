#ifndef MU_PLAYER_RT
#define MU_PLAYER_RT

#include "mu2.h"
#include "mu_types.h"
#include "RtAudio.h"
#include "player2.h"
#include "Stk.h"

namespace mu {

  class PlayerRt : public Player2 {

  public:

    PlayerRt( void );
    ~PlayerRt( void );

    static const int default_device_number() {
      return 0;
    }

    int device_number() { 
      return device_number_;
    }

    void set_device_number(int device_number) {
      device_number_ = device_number;
    }

    void start();
    void stop();

    enum RtAudioDirective { 
      kContinue = 0, 
      kStopAndDrain = 1, 
      kStopImmediately = 2 };
    
    // callback method for RtAudio
    RtAudioDirective processBuffer(void *buffer, unsigned int frame_count, double stream_time);
    
  protected:
    int device_number_;
    RtAudio dac_;
    stk::StkFrames stk_frames_;
    
  };

}

#endif
