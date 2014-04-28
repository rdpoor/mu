#include "looper.h"

namespace mu {

  Looper::~Looper() {
    TRACE("Looper::~Looper()\n");
  }
  
  // Pass samples through from the input until the time reaches
  // loop_start + loop_duration.  Then, shift time to loop the input
  // at start_time + N*loop_duration, where N is the number of loops
  // that have happened.
  //
  // If t is the input time, then the time advertized to the source
  // is (t < loop_start) ? t : loop_start + (t % loop_duration).

  Looper& Looper::step(stk::StkFrames& buffer, 
                       MuTime time,
                       const Transport& transport) {
    // src_time is the time of the sample written into buffer[0]
    MuTime src_time = (time < loop_start_) ? time : loop_start_ + (time % loop_duration_);

    return *this;
  }

}
