/*
 * Looper takes an input stream and causes it to repeat every
 * loopDuration seconds.
 */
#ifndef MU_LOOPER_H
#define MU_LOOPER_H

#include "mu.h"
#include "node.h"

namespace mu {

  class Looper : public Node {
  public:

    Looper();
    ~Looper( void );

    Looper& step(stk::StkFrames& buffer, 
                 Tick tick,
                 const Player &player);
    Tick streamDuration();

    Tick getLoopDuration();
    Looper& setLoopDuration(Tick loop_duration);

    Node *getSource();
    Looper& setSource(Node *source);

  protected:
    Tick loop_duration_;
    stk::StkFrames buffer_;
    Node *source_;

  };                            // class Looper

  inline Looper::Looper()
    : loop_duration_ (1.0) {
    TRACE("Looper::Looper()\n");
  }

  inline Tick Looper::streamDuration() { return kIndefinite; }

  inline Tick Looper::getLoopDuration() { return loop_duration_; }
  inline Looper& Looper::setLoopDuration(Tick loop_duration) {
    TRACE("Looper::loopDuration()\n");
    loop_duration_ = loop_duration; return *this;
  }

  inline Node *Looper::getSource() { return source_; }
  inline Looper& Looper::setSource(Node *source) {
    TRACE("Looper::source()\n");
    source_ = source; return *this;
  }
  

}                               // namespace mu

#endif
