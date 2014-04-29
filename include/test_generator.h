/*
 * TestGenerator is a source of test data.  It simply outputs
 * a value of N for the N'th sample.
 */
#ifndef MU_TEST_GENERATOR_H
#define MU_TEST_GENERATOR_H

#include "mu.h"
#include "node.h"

namespace mu {

  class TestGenerator : public Node {
  public:

    TestGenerator();
    ~TestGenerator( void );
    TestGenerator& step(stk::StkFrames& buffer, 
                   MuTime time, 
                   const Player& player);
    MuTime streamDuration();
  };                            // class TestGenerator

}                               // namespace mu

#endif
