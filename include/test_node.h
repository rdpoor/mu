#ifndef MU_TEST_NODE_H
#define MU_TEST_NODE_H

#include "mu.h"
#include "node.h"

namespace mu {

  class TestNode : public Node {
  public:

    TestNode();
    ~TestNode( void );
    TestNode& step(stk::StkFrames& buffer, 
                   MuTime time, 
                   const Transport& transport);
    MuTime streamDuration();

  protected:
    long int frame_index_;

  };                            // class TestNode

}                               // namespace mu

#endif
