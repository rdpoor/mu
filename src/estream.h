/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

  ================================================================
*/

/*
 * EStream: Indefinite Streams of Events
 *
 * A specialization of IStream, an EStream always contains Event objects.  An
 * Event object always has a time() field, and EStreams always emit Events in
 * ascending time order.
 */

#ifndef MU_ESTREAM_H
#define MU_ESTREAM_H

#include "event.h"
#include "istream.h"

namespace mu {

  class EStream : public IStream<Event *> {
  public:
    Time t() { return first()->time(); }
    EStream *rest() { return (EStream *)IStream::rest(); }
    Event *first() { return (Event *)IStream::first(); }

    static EStream *Merge(EStream *s0, EStream *s1) {
      return (EStream *)IStream::Merge(s0,
                                       s1,
                                       [=](Event *e0, Event *e1) {
                                         return e0->time() < e1->time();
                                       });
    }

    static EStream *Metronome(double delta) {
      return Metronome(0.0, delta);
    }

    static EStream *Metronome(double start, double delta) {
      return (EStream *)ISTREAM_CREATE(new Event(start),
                                       Metronome(start + delta, delta),
                                       Event *);
    }
    
  };
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
