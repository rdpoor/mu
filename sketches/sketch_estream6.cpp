/*
 * side-effect free streams.  Uses lambda to create deferred evaluation.
 * Specialized streams are static methods of the general class.
 */

#include <functional>
#include <string>
#include <vector>

#define NULL_ESTREAM nullptr
#define IS_NULL_ESTREAM(s) ((s) == NULL_ESTREAM)
#define DEFER(form, T) [=]()->EStream<T>*{return (form);}
#define FORCE(deferred_form) (deferred_form())
#define CREATE_STREAM(first, rest, T) new EStream<T>((first), DEFER(rest, T))

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
    return IS_NULL_ESTREAM(rest_) ? NULL_ESTREAM : FORCE(rest_); 
  }

  // ================================================================
  // utilities

  static std::vector<T> *to_vector(EStream<T> *s, int maxcount) {
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

  template <typename MapFn>
  static EStream<T> *MapES(EStream<T> *s, MapFn mapfn) {
    if (IS_NULL_ESTREAM(s)) {
      return NULL_ESTREAM;
    } else {
      return CREATE_STREAM(mapfn(s->first()), MapES(s->rest(), mapfn), T);
    }
  }

  template <typename PredFn>
  static EStream<T> *MergeES(EStream<T> *s0, EStream<T> *s1, PredFn predfn) {
    if (IS_NULL_ESTREAM(s0)) {
      return s1;
    } else if (IS_NULL_ESTREAM(s1)) {
      return s0;
    } else if (predfn(s0->first(), s1->first())) {
      return CREATE_STREAM(s0->first(),
                           MergeES(s0->rest(), s1, predfn),
                           T);
    } else {
      return CREATE_STREAM(s1->first(),
                           MergeES(s0, s1->rest(), predfn),
                           T);
    }
  }

  static EStream<T> *TakeES(EStream<T> *s, int n) {
    if ((IS_NULL_ESTREAM(s)) || (n <= 0)) {
      return NULL_ESTREAM;
    } else {
      return CREATE_STREAM(s->first(), TakeES(s->rest(), n-1), T);
    }
  }
  
};

// ================================================================
// specialize streams

EStream<int> *IntegersES(int starting) {
  return CREATE_STREAM(starting, IntegersES(starting + 1), int);
}

// ================================================================
// main

int main( void ) {

  EStream<int> *take0 = EStream<int>::TakeES(IntegersES(0), 4);
  std::vector<int> *v0actual = EStream<int>::to_vector(take0, 1000);
  std::vector<int> v0expected = { 0, 1, 2, 3 };
  printf("take0: %s\n", *v0actual == v0expected ? "match" : "mismatch");

  EStream<int> *append1 = 
    EStream<int>::AppendES(EStream<int>::TakeES(IntegersES(0), 4), 
                           IntegersES(100));
  std::vector<int> *v1actual = EStream<int>::to_vector(append1, 7);
  std::vector<int> v1expected = { 0, 1, 2, 3, 100, 101, 102 };
  printf("append1: %s\n", *v1actual == v1expected ? "match" : "mismatch");

  EStream<int> *s3 = EStream<int>::MapES(IntegersES(0), 
                                         [=](int v) { return v * 3; });
  std::vector<int> *s3actual = EStream<int>::to_vector(s3, 7);
  std::vector<int> s3expected = { 0, 3, 6, 9, 12, 15, 18 };
  printf("MapES: %s\n", *s3actual == s3expected ? "match" : "mismatch");
  

  return 0;
}
