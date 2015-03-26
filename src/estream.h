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
 * Indifinite Streams
 */

#ifndef MU_ESTREAM_H
#define MU_ESTREAM_H

#include <functional>
#include <string>
#include <vector>

namespace mu {

#define NULL_ESTREAM nullptr
#define IS_NULL_ESTREAM(s) ((s) == NULL_ESTREAM)
#define ESTREAM_DEFER(form, T) [=]()->EStream<T>*{return (form);}
#define ESTREAM_FORCE(deferred_form) (deferred_form())
#define CREATE_STREAM(first, rest, T) \
        new EStream<T>((first), ESTREAM_DEFER(rest, T))

  template <typename T>class EStream {
  protected:
    T first_;
    std::function<EStream<T> *()> rest_;
    
  public:
    EStream(T first, std::function<EStream<T> *()> rest) :
      first_(first), rest_(rest) {}
    
    T first() { 
      return first_; 
    }
    
    EStream<T> *rest() { 
      return IS_NULL_ESTREAM(rest_) ? NULL_ESTREAM : ESTREAM_FORCE(rest_); 
    }
    
    // ================================================================
    // utilities
    
    static std::vector<T> *to_vector(EStream<T> *s, int maxcount = 100) {
      std::vector<T> *v = new std::vector<T>();
      
      EStream<T> *s0 = s;
      while (!IS_NULL_ESTREAM(s0) && (maxcount > 0)) {
        v->push_back(s0->first());
        EStream<T> *next = s0->rest();
        if (s0 != s) {            // don't delete caller's stream!
          delete s0;
        }
        s0 = next;
        maxcount -= 1;
      }
      return v;
    }
    
    
    // Static methods from here on
    
    static EStream<T> *AppendES(EStream<T> *s0, EStream<T> *s1) {
      if (IS_NULL_ESTREAM(s0)) {
        return s1;
      } else if (IS_NULL_ESTREAM(s1)) {
        return s0;
      } else {
        return CREATE_STREAM(s0->first(), AppendES(s0->rest(), s1), T);
      }
    }

    template <typename PredFn>
    static EStream<T> *FilterES(EStream<T> *s, PredFn predfn) {
      if (IS_NULL_ESTREAM(s)) {
        return NULL_ESTREAM;
      } else if (predfn(s->first())) {
        return CREATE_STREAM(s->first(), 
                             FilterES(s->rest(), predfn), 
                             T);
      } else {
        return FilterES(s->rest(), predfn);
      }
    }

    static EStream<T> *IncrementES(T initial) {
      return CREATE_STREAM(initial, IncrementES(initial + 1), T);
    }

    template <typename MapFn>
    static EStream<T> *MapES(EStream<T> *s, MapFn mapfn) {
      if (IS_NULL_ESTREAM(s)) {
        return NULL_ESTREAM;
      } else {
        return CREATE_STREAM(mapfn(s->first()), MapES(s->rest(), mapfn), T);
      }
    }
    
    template <typename SortFn>
    static EStream<T> *MergeES(EStream<T> *s0, EStream<T> *s1, SortFn sortfn) {
      if (IS_NULL_ESTREAM(s0)) {
        return s1;
      } else if (IS_NULL_ESTREAM(s1)) {
        return s0;
      } else if (sortfn(s0->first(), s1->first())) {
        return CREATE_STREAM(s0->first(),
                             MergeES(s0->rest(), s1, sortfn),
                             T);
      } else {
        return CREATE_STREAM(s1->first(),
                             MergeES(s0, s1->rest(), sortfn),
                             T);
      }
    }
    
    static EStream<T> *RepeatES(EStream<T> *s) {
      if (IS_NULL_ESTREAM(s)) {
        return NULL_ESTREAM;
      } else {
        return RepeatES_aux(s, s);
      }
    }

  private:
    static EStream<T> *RepeatES_aux(EStream<T> *s, EStream *s0) {
      if (IS_NULL_ESTREAM(s)) {
        return RepeatES_aux(s0, s0);
      } else {
        return CREATE_STREAM(s->first(), RepeatES_aux(s->rest(), s0), T);
      }
    }

  public:
    static EStream<T> *TakeES(EStream<T> *s, int n) {
      if ((IS_NULL_ESTREAM(s)) || (n <= 0)) {
        return NULL_ESTREAM;
      } else {
        return CREATE_STREAM(s->first(), TakeES(s->rest(), n-1), T);
      }
    }
    
  };
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
