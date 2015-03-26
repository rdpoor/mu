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
 * IStream: Indefinite Streams
 *
 * Styled after SICP streams (chapter 3), an IStream has a "first" and a "rest".
 * The first has a value, the rest contains a delayed form that when evaluated
 * yields the next element in the stream.
 */

#ifndef MU_ISTREAM_H
#define MU_ISTREAM_H

#include <functional>
#include <string>
#include <vector>

namespace mu {

  // definition of an empty stream
#define NULL_ISTREAM nullptr

  // predicate to test for an empty stream
#define IS_NULL_ISTREAM(s) ((s) == NULL_ISTREAM)

  // defer evaluation of `form` until forced
#define ISTREAM_DEFER(form, T) [=]()->IStream<T>*{return (form);}

  // force the evaluation of a deferred form
#define ISTREAM_FORCE(deferred_form) (deferred_form())

  // create a stream, deferring the evaluation of `rest`
#define ISTREAM_CREATE(first, rest, T) \
  new IStream<T>((first), ISTREAM_DEFER(rest, T))

  template <typename T>class IStream {
  protected:
    T first_;
    std::function<IStream<T> *()> rest_;
    
  public:
    IStream(T first, std::function<IStream<T> *()> rest) :
      first_(first), rest_(rest) {}
    
    T first() { 
      return first_; 
    }
    
    IStream<T> *rest() { 
      return ISTREAM_FORCE(rest_); 
    }
    
    // ================================================================
    // utilities
    
    static std::vector<T> *to_vector(IStream<T> *s, int maxcount = 100) {
      std::vector<T> *v = new std::vector<T>();
      
      IStream<T> *s0 = s;
      while (!IS_NULL_ISTREAM(s0) && (maxcount > 0)) {
        v->push_back(s0->first());
        IStream<T> *next = s0->rest();
        if (s0 != s) {            // don't delete caller's stream!
          delete s0;
        }
        s0 = next;
        maxcount -= 1;
      }
      return v;
    }
    
    
    // Static methods from here on
    
    static IStream<T> *AppendIS(IStream<T> *s0, IStream<T> *s1) {
      if (IS_NULL_ISTREAM(s0)) {
        return s1;
      } else if (IS_NULL_ISTREAM(s1)) {
        return s0;
      } else {
        return ISTREAM_CREATE(s0->first(), AppendIS(s0->rest(), s1), T);
      }
    }

    template <typename PredFn>
    static IStream<T> *FilterIS(IStream<T> *s, PredFn predfn) {
      if (IS_NULL_ISTREAM(s)) {
        return NULL_ISTREAM;
      } else if (predfn(s->first())) {
        return ISTREAM_CREATE(s->first(), 
                             FilterIS(s->rest(), predfn), 
                             T);
      } else {
        return FilterIS(s->rest(), predfn);
      }
    }

    static IStream<T> *IncrementIS(T initial) {
      return ISTREAM_CREATE(initial, IncrementIS(initial + 1), T);
    }

    template <typename MapFn>
    static IStream<T> *MapIS(IStream<T> *s, MapFn mapfn) {
      if (IS_NULL_ISTREAM(s)) {
        return NULL_ISTREAM;
      } else {
        return ISTREAM_CREATE(mapfn(s->first()), MapIS(s->rest(), mapfn), T);
      }
    }
    
    template <typename SortFn>
    static IStream<T> *MergeIS(IStream<T> *s0, IStream<T> *s1, SortFn sortfn) {
      if (IS_NULL_ISTREAM(s0)) {
        return s1;
      } else if (IS_NULL_ISTREAM(s1)) {
        return s0;
      } else if (sortfn(s0->first(), s1->first())) {
        return ISTREAM_CREATE(s0->first(),
                             MergeIS(s0->rest(), s1, sortfn),
                             T);
      } else {
        return ISTREAM_CREATE(s1->first(),
                             MergeIS(s0, s1->rest(), sortfn),
                             T);
      }
    }
    
    static IStream<T> *RepeatIS(IStream<T> *s) {
      if (IS_NULL_ISTREAM(s)) {
        return NULL_ISTREAM;
      } else {
        return RepeatIS_aux(s, s);
      }
    }

  private:
    static IStream<T> *RepeatIS_aux(IStream<T> *s, IStream *s0) {
      if (IS_NULL_ISTREAM(s)) {
        return RepeatIS_aux(s0, s0);
      } else {
        return ISTREAM_CREATE(s->first(), RepeatIS_aux(s->rest(), s0), T);
      }
    }

  public:
    static IStream<T> *TakeIS(IStream<T> *s, int n) {
      if ((IS_NULL_ISTREAM(s)) || (n <= 0)) {
        return NULL_ISTREAM;
      } else {
        return ISTREAM_CREATE(s->first(), TakeIS(s->rest(), n-1), T);
      }
    }
    
  };
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
