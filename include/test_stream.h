/*
 * TestStream is a source of test data.  It simply outputs
 * a value of F at frame F.
 */
#ifndef MU_TEST_STREAM_H
#define MU_TEST_STREAM_H

#include "mu.h"
#include "node.h"

namespace mu {

  class TestStream : public Node {
  public:

    TestStream();
    ~TestStream( void );
    TestStream& step(stk::StkFrames& buffer, 
                     Tick tick,
                     Player& player);
    Tick streamDuration();
  };                            // class TestStream

}                               // namespace mu

#endif
