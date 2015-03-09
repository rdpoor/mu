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

// File: mu_scheduler.h
//
// MuScheduler holds MuDeferredEvent objects and calls their underlying
// actions at the appropriate time.

#ifndef MU_SCHEDULER_H
#define MU_SCHEDULER_H

#include "mu_deferred_event.h"
#include "mu_types.h"
#include <mutex>
#include <stdlib.h>
#include <vector>

namespace mu {

  class MuScheduler {

  public:
    MuScheduler( void ) : current_event_(NULL) {}

    virtual ~MuScheduler( void );
    
    size_t event_count() {
      return queue_.size();
    }

    // Add a timed event to the queue.
    void schedule_event(MuTick time, DeferredAction action);

    // Remove all the events from the queue.
    void clear_all_events( void ) {
      mutex_.lock();
      // TODO: does clear() delete the events?
      queue_.clear();
      mutex_.unlock();
    }

    // Return the event currently being processed (i.e. within the
    // scope of a call to step()), or NULL if there is no current
    // event.
    MuDeferredEvent *current_event() {
      return current_event_;
    }

    // Return the time of the event currently being processed, or
    // kUndefined if there is no current event.
    MuTick current_event_time() {
      return (current_event_ == NULL) ? kUndefined : current_event_->time();
    }

    // Return the next event in the queue, or NULL if there are no
    // more events.
    MuDeferredEvent *next_event() {
      return get_next_event(false);
    }

    // Return the time of the next event in the queue, or kUndefined
    // if there are no more events.
    MuTick next_event_time() {
      MuDeferredEvent *deferred_event = next_event();
      return (deferred_event == NULL) ? kUndefined : deferred_event->time();
    }

    // Remove and process the next event in the queue.  Returns false
    // if the queue was empty.
    bool step() {
      // TODO: ASSERT(current_event_ == NULL)
      current_event_ = get_next_event(true);

      if (current_event_ == NULL) {
	return false;
      } else {
	current_event_->call();
	delete current_event_;
	current_event_ = NULL;
	return true;
      }
    }

  protected:

    MuDeferredEvent *get_next_event(bool remove) {
      MuDeferredEvent *deferred_event = NULL;
      mutex_.lock();
      if (event_count() > 0) {
	deferred_event = queue_.back();
	if (remove) queue_.pop_back();
      }
      mutex_.unlock();
      return deferred_event;
    }

    std::vector<MuDeferredEvent *> queue_;
    std::mutex mutex_;
    MuDeferredEvent *current_event_;

  };                            // class MuScheduler

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
