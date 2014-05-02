/*
 * ConstantStream is a source of constant values.  It simply outputs a
 * fixed value for every frame.
 */
#ifndef MU_CONSTANT_STREAM_H
#define MU_CONSTANT_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class ConstantStream : public Stream {
  public:

    ConstantStream( void );
    ~ConstantStream( void );
    ConstantStream& step(stk::StkFrames& buffer, Tick tick, Player& player);
    stk::StkFloat getValue() const { return value_; }
    ConstantStream& setValue(stk::StkFloat value) { value_ = value; return *this; }
    
  protected:
    stk::StkFloat value_;
  };                            // class ConstantStream
}                               // namespace mu
#endif
