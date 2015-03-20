#include <iostream>
#include <vector>

namespace motif {

#define TEST_POLYMORPHIC_EVENTS
#ifdef TEST_POLYMORPHIC_EVENTS
  // ================================================================
  // the following compiles, but do I want it?
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
  // ================================================================
  // create iterators for infinite sequences
    
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
