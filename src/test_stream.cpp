#include "test_stream.h"

namespace mu {

  TestStream::TestStream() {
    TRACE("TestStream::TestStream()\n");
  }
  
  TestStream::~TestStream() {
    TRACE("TestStream::~TestStream()\n");
  }
  
  TestStream& TestStream::step(stk::StkFrames& buffer, 
                               Tick tick,
                               const Player &player) {
    Tick frame_count = buffer.frames();
    Tick channel_count = buffer.channels();

    Tick f = tick;
    switch (channel_count) {
    case 1:
      for (Tick i=0; i<frame_count; i++, f++) {
        buffer[i] = f;
      }
      break;
    case 2:
      for (Tick i=0; i<frame_count; i++, f++) {
        buffer[2*i] = f;
        buffer[2*i+1] = f;
      }
      break;
    default:
      for (Tick i=0; i<frame_count; f++) {
        for (unsigned chan = 0; chan<channel_count; chan++) {
          buffer[channel_count * i + chan] = f;
        }
      }
    }
    return *this;
  }

  Tick TestStream::streamDuration() {
    TRACE("TestStream::streamDuration()\n");
    return kIndefinite;
  }

}
