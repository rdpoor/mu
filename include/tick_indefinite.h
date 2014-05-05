#ifndef MU_TICK_INDEFINITE_H
#define MU_TICK_INDEFINITE_H

namespace mu {

  class TickIndefinite {
  public:

    static const TickIndefinite kIndefinite;

    TickIndefinite() {}
    ~TickIndefinite() {}

  };

  inline TickIndefinite operator+ (long int i, TickIndefinite t) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator+ (TickIndefinite t, long int i) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator+ (TickIndefinite t1, TickIndefinite t2) { return TickIndefinite::kIndefinite; }

  inline TickIndefinite operator- (long int i, TickIndefinite t) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator- (TickIndefinite t, long int i ) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator- (TickIndefinite t1, TickIndefinite t2) { return TickIndefinite::kIndefinite; }

  inline TickIndefinite operator* (long int i, TickIndefinite t) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator* (TickIndefinite t, long int i ) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator* (TickIndefinite t1, TickIndefinite t2) { return TickIndefinite::kIndefinite; }

  inline long int operator/ (long int i, TickIndefinite t) { return 0; }
  inline TickIndefinite operator/ (TickIndefinite t, long int i ) { return TickIndefinite::kIndefinite; }
  inline TickIndefinite operator/ (TickIndefinite t1, TickIndefinite t2) { return TickIndefinite::kIndefinite; }

  inline bool operator> (long int i, TickIndefinite t) { return false; }
  inline bool operator> (TickIndefinite t, long int i) { return true; }
  inline bool operator> (TickIndefinite t1, TickIndefinite t2) { return false; }

  inline bool operator>= (long int i, TickIndefinite t) { return false; }
  inline bool operator>= (TickIndefinite t, long int i) { return true; }
  inline bool operator>= (TickIndefinite t1, TickIndefinite t2) { return true; }

  inline bool operator< (long int i, TickIndefinite t) { return true; }
  inline bool operator< (TickIndefinite t, long int i) { return false; }
  inline bool operator< (TickIndefinite t1, TickIndefinite t2) { return false; }

  inline bool operator<= (long int i, TickIndefinite t) { return true; }
  inline bool operator<= (TickIndefinite t, long int i) { return false; }
  inline bool operator<= (TickIndefinite t1, TickIndefinite t2) { return true; }

  inline bool operator== (long int i, TickIndefinite t) { return false; }
  inline bool operator== (TickIndefinite t, long int i) { return false; }
  inline bool operator== (TickIndefinite t1, TickIndefinite t2) { return true; }

  inline bool operator!= (long int i, TickIndefinite t) { return true; }
  inline bool operator!= (TickIndefinite t, long int i) { return true; }
  inline bool operator!= (TickIndefinite t1, TickIndefinite t2) { return false; }

}

    
#endif
