#include <iostream>
#include <vector>

namespace motif {

#define TEST_POLYMORPHIC_EVENTS
#ifdef TEST_POLYMORPHIC_EVENTS
  // ================================================================
  // the following compiles, but do I want it?  The idea is that a compound
  // event can contain a placed atomic event or a placed compount event (turtles
  // all the way down).
  //
  // see also
  // https://github.com/petermichaux/bootstrap-scheme/blob/v0.21/scheme.c

  class PlacedEvent;
  class Properties;
  
  class AtomicEvent {
  public:
    double start();
    double end();
    Properties properties();
  };

  class CompoundEvent {
  public:
    std::vector<PlacedEvent &> events();
  };
    
  typedef enum { ATOMIC_EVENT, COMPOUND_EVENT } event_type;

  class Event {
  public:
    event_type type;
    union {
      AtomicEvent atomic_event_;
      CompoundEvent compound_event_;
    };
  };

  class PlacedEvent : public Event {
    double offset();
  };
#endif	// TEST_POLYMORPHIC_EVENTS
  
#define TEST_INFINITE_SEQUENCES
#ifdef TEST_INFINITE_SEQUENCES

#include <iterator>

  // ================================================================
  // create iterators for infinite sequences
  //
  // Good examples of custom iterator:
  // http://codereview.stackexchange.com/questions/57351/custom-iterator-implementation-returning-opencv-mat
  // http://www.cprogramming.com/c++11/c++11-ranged-for-loop.html
  //
  // Aha: the seek_to() method is really just a factory that generates an
  // iterator.  The iterator embodies all the logic for head (*) and next (++).
  //
  // General form:
  //   it = specialized_stream.seek_to(time);
  //   while (it != specialized_stream.end()) {
  //     std::cout << *it << std::endl;
  //     ++it;
  //   }
    
  class Properties {
  };
    
  class SequenceEvent {
  public:
    double time();
    Properties properties();
  };

  class SequenceIterator {
  public:
    // these three operators allow us to create basic loops:
    // for (it = my_sequence().begin; it != my_sequence().end; ++it) {
    //   std::cout << ' ' << *it;
    // }
    SequenceEvent& operator*();
    const SequenceIterator& operator++();
    bool operator!=(SequenceIterator& rhs);
  };

  class Sequence {
  public:
    SequenceIterator begin();
    SequenceIterator end();
  };

#endif	// TEST_INFINITE_SEQUENCES
  
  
} // namespace motif

int main() {

#ifdef TEST_INFINITE_SEQUENCES
  motif::Sequence ss;
  for (motif::SequenceIterator p = ss.begin(); p != ss.end(); ++p) {
    std::cout << *p << std::endl;
  }
#endif	// TEST_INFINITE_SEQUENCES

  return 0;
}
