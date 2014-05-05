/*
 * LinsegStream generates linearly interpolated samples beween given
 * [tic,y] breakpoints.
 */
#ifndef MU_LINSEG_STREAM
#define MU_LINSEG_STREAM

#include "mu.h"
#include "stream.h"
#include <map>

namespace mu {

  typedef std::map<mu::Tick, stk::StkFloat> Breakpoints;

  class LinsegStream : public Stream {
  public:
    LinsegStream& step(stk::StkFrames& buffer, Tick tick, Player &player);
    
    Tick getStart( void );
    Tick getEnd( void );

    LinsegStream& addBreakpoint(Tick tick, stk::StkFloat value);
    LinsegStream& removeAllBreakpoints();
    size_t getBreakpointCount() { return breakpoints_.size(); }

    bool getHoldValue() { return hold_value_; }
    LinsegStream& setHoldValue(bool hold_value) { hold_value_ = hold_value; return *this; }

  private:
    stk::StkFloat lerp(Tick tick);

  protected:
    std::map<Tick, stk::StkFloat> breakpoints_;
    bool hold_value_;
  };

}

#endif

