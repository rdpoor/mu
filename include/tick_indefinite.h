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

// Local Variables:
// mode: c++
// End:
