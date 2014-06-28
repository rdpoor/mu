/*
 * FadeSP fades the source stream in and out.  
 *
 * If getStart() is indefinite, fade in starts at source->getStart().
 * If getStart() is definite, then fade in starts at getStart() -
 * getFadeTime() / 2 or at source->getStart(), whichever is greater.
 *
 * If getEnd() is indefinite, fade out ends at source->getEnd().
 * If getEnd() is definite, then fade out ends at getStart() +
 * getFadeTime() / 2 or at source->getEnd(), whichever is less.

 */
/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/

#ifndef MU_FADE_STREAM_H
#define MU_FADE_STREAM_H

#include "mu.h"
#include "single_source_sp.h"

namespace mu {

  class FadeSP : public SingleSourceSP<FadeSP> {
  public:

    static const Tick kDefaultFadeTime = 0;

    FadeSP( void );
    ~FadeSP( void );

    std::string getClassName() { return "FadeSP"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, bool is_new_event);

    Tick getStart( void );
    FadeSP& setStart(Tick start) {  start_ = start; return *this; }

    Tick getEnd( void );
    FadeSP& setEnd(Tick end) { end_ = end; return *this; }

    Tick getFadeTime( void ) { return fade_time_; }
    FadeSP& setFadeTime(Tick fade_time) { fade_time_ = fade_time; return *this; }

  protected:
    Tick start_;
    Tick end_;
    Tick fade_time_;
  };                            // class FadeSP

  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
