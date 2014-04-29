/*
 * TestBuzz creates a stereo pulse wave.
 */
#ifndef MU_TEST_BUZZ_H
#define MU_TEST_BUZZ_H

#include "mu.h"
#include "node.h"

namespace mu {

  class TestBuzz : public Node {
  public:

    TestBuzz();
    ~TestBuzz( void );
    TestBuzz& step(stk::StkFrames& buffer, 
                   Tick tick,
                   Player& player);
    Tick streamDuration();

  protected:
    Tick frame_index_;

  };                            // class TestStream

}                               // namespace mu

#endif
