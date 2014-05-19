/*
 * AddStream mixes N sources together.
 */
#ifndef MU_ADD_STREAM_H
#define MU_ADD_STREAM_H

#include "mu.h"
#include "multi_source_stream.h"

namespace mu {

  class AddStream : public MultiSourceStream {
  public:

    AddStream();
    ~AddStream( void );

    std::string getClassName() { return "AddStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    AddStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

  };                            // class AddStream

}                               // namespace mu

#endif
