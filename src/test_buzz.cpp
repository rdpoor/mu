#include "test_buzz.h"

namespace mu {

  TestBuzz::TestBuzz() 
    : frame_index_ (0) {
    TRACE("TestBuzz::TestBuzz()\n");
  }
  
  TestBuzz::~TestBuzz() {
    TRACE("TestBuzz::~TestBuzz()\n");
  }
  
  TestBuzz& TestBuzz::step(stk::StkFrames& buffer, 
                           Tick tick,
                           Player &player) {
    long int frame_count = buffer.frames();
    int channel_count = buffer.channels();
    int i = ((frame_index_ * 2) + 1) %  (frame_count * channel_count);
    // The buffer does not arrive empty...
    bzero(&(buffer[0]), frame_count * channel_count * sizeof(stk::StkFloat));
    buffer[0] = 0.9;            // exciting waveform...
    buffer[i] = 0.9;
    frame_index_ += 3;
    // fprintf(stderr, "step: %p %f %d\r", &buffer, time, channel_count);
    return *this;
  }

  Tick TestBuzz::streamDuration() {
    TRACE("TestBuzz::streamDuration()\n");
    return kIndefinite;
  }

}
