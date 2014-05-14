/*
 * IdentityStream is a source of test data.  It simply outputs
 * a value of F at frame F.
 */
#ifndef MU_IDENTITY_STREAM_H
#define MU_IDENTITY_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class IdentityStream : public Stream {
  public:

    IdentityStream();
    ~IdentityStream( void );
    std::string getClassName() { return "IdentityStream"; }
    IdentityStream& step(stk::StkFrames& buffer, 
                     Tick tick,
                     Player& player);
  };                            // class IdentityStream

}                               // namespace mu

#endif
