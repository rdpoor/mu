#include "test_node.h"

namespace mu {

  TestNode::TestNode() 
    : frame_index_ (0) {
    TRACE("TestNode::TestNode()\n");
  }
  
  TestNode::~TestNode() {
    TRACE("TestNode::~TestNode()\n");
  }
  
  TestNode& TestNode::step(stk::StkFrames& buffer, 
                           MuTime time,
                           const Transport &transport) {
    long int frame_count = buffer.frames();
    int channel_count = buffer.channels();
    int i = ((frame_index_ * 2) + 1) %  (frame_count * channel_count);
    // The buffer does not arrive empty...
    bzero(&(buffer[0]), frame_count * channel_count * sizeof(stk::StkFloat));
    buffer[0] = 0.9;            // exciting waveform...
    buffer[i] = 0.9;
    frame_index_ += 3;
    // fprintf(stderr, "step: %p %d %d\r", &buffer, frame_count, channel_count);
    return *this;
  }

  MuTime TestNode::duration() {
    TRACE("TestNode::duration()\n");
    return INDEFINITE;
  }

  TestNode& TestNode::acquireResources() {
    TRACE("TestNode::acquireResources()\n");
    return *this;
  }

  TestNode& TestNode::releaseResources() {
    TRACE("TestNode::releaseResources()\n");
    return *this;
  }

  TestNode& TestNode::seek(MuTime time) {
    TRACE("TestNode::seek()\n");
    return *this;
  }

}
