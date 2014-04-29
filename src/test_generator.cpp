#include "test_generator.h"

namespace mu {

  TestGenerator::TestGenerator() {
    TRACE("TestGenerator::TestGenerator()\n");
  }
  
  TestGenerator::~TestGenerator() {
    TRACE("TestGenerator::~TestGenerator()\n");
  }
  
  TestGenerator& TestGenerator::step(stk::StkFrames& buffer, 
                           MuTime time,
                           const Player &player) {
    unsigned int starting_frame = (unsigned int)(time * buffer.dataRate());
    unsigned int frame_count = buffer.frames();
    unsigned int channel_count = buffer.channels();

    unsigned int f = starting_frame;
    switch (channel_count)
    case 1:
      for (unsigned int i=0; i<frame_count; i++, f++) {
        buffer[i] = f;
      }
      break;
    case 2:
      for (unsigned int i=0; i<frame_count; i++, f++) {
        buffer[2*i] = f;
        buffer[2*i+1] = f;
      }
      break;
    default:
      for (unsigned int i=0; i<frame_count; f++) {
        for (unsigned chan = 0; chan<channel_count; chan++) {
          buffer[channel_count * i + chan] = f;
        }
      }
    }
    return *this;
  }

  MuTime TestGenerator::streamDuration() {
    TRACE("TestGenerator::streamDuration()\n");
    return INDEFINITE;
  }

}
