/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  

   ================================================================ 
*/

// File: mu_deferred_event.h
//
// MuTimedEvent encapsulates a deferred action to be performed at a
// specific time.

#ifndef MU_DEFERRED_EVENT_H
#define MU_DEFERRED_EVENT_H

#include "mu_types.h"

namespace mu {

  typedef std::function<void ( void )> DeferredAction;

  class MuDeferredEvent {

  public:
    MuDeferredEvent( void ) : time_(kUndefined), action_(NULL) {}

    MuTick time() { return time_; }
    void set_time(MuTick time) { time_ = time; }

    DeferredAction action( void ) { return action_; }
    void set_action(DeferredAction action) { action_ = action; }

    // perform the deferred action now
    void call() {
      if (action_) action_();
    }

  protected:
    MuTick time_;
    DeferredAction action_;
  };                            // class MuEvent

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
