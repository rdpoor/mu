#ifndef MU_LOOPER_H
#define MU_LOOPER_H

#include "mu.h"
#include "node.h"

namespace mu {

  class Looper : public Node {
  public:

    Looper();
    Looper& loopStart(MuTime loop_start);
    Looper& loopDuration(MuTime loop_duration);
    Looper& source(Node *source);

    ~Looper( void );

    Looper& step(stk::StkFrames& buffer, 
                 MuTime time, 
                 const Transport &transport);
    MuTime streamDuration();

    MuTime getStartTime();
    MuTime getLoopDuration();
    Node *getSource();

  protected:
    MuTime loop_start_;
    MuTime loop_duration_;
    Node *source_;

  };                            // class Looper

  inline Looper::Looper()
    : loop_start_ (0.0), loop_duration_ (1.0) {
    TRACE("Looper::Looper()\n");
  }

  inline Looper& Looper::loopStart(MuTime loop_start) {
    TRACE("Looper::loopStart()\n");
    loop_start_ = loop_start; return *this;
  }

  inline Looper& Looper::loopDuration(MuTime loop_duration) {
    TRACE("Looper::loopDuration()\n");
    loop_duration_ = loop_duration; return *this;
  }

  inline Looper& Looper::source(Node *source) {
    TRACE("Looper::source()\n");
    source_ = source; return *this;
  }
  
  inline MuTime Looper::streamDuration() { 
    return (double)INDEFINITE;
  }

  inline MuTime Looper::getStartTime() { 
    return loop_start_;
  }

  inline MuTime Looper::getLoopDuration() { 
    return loop_duration_;
  }

  inline Node *Looper::getSource() { return source_; }

}                               // namespace mu

#endif
