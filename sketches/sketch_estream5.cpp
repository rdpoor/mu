/*
 * side-effect free streams.  Uses lambda to create deferred evaluation.
 */

#include <functional>
#include <string>

#define NULL_ESTREAM(T) ((EStream<T> *)0)
#define CREATE_STREAM(first, rest, T) \
  new EStream<T>((first), [=]()->EStream<T>*{return (rest);})

template <typename T>class EStream {
public:
  EStream(T first, std::function<EStream<T> *()> rest) :
    first_(first), rest_(rest) {}
  T first() { return first_; }
  EStream<T> *rest() { return rest_ ? rest_() : NULL_ESTREAM(T); }
protected:
  T first_;
  std::function<EStream<T> *()> rest_;
};

// Specialize streams below, arranged alphabetically

template <typename T>
EStream<T> *AppendES(EStream<T> *s0, EStream<T> *s1) {
  if (s0 == NULL_ESTREAM(T)) {
    return s1;
  } else if (s1 == NULL_ESTREAM(T)) {
    return s0;
  } else {
    return CREATE_STREAM(s0->first(),
                         AppendES(s0->rest(), s1),
                         T);
  }
}

template <typename T>
EStream<T> *ConstantES(T value) {
  return CREATE_STREAM(value, ConstantES(value), T);
}

template <typename T>
EStream<T> *CycleES(EStream<T> *s) {
  return CycleESAux(s, s);
}

template <typename T>
EStream<T> *CycleESAux(EStream<T> *s, EStream<T> *s0) {
  if (s == NULL_ESTREAM(T)) {
    return CREATE_STREAM(s0->first(),
                         CycleESAux(s0->rest(), s0), T);
  } else {
    return CREATE_STREAM(s->first(),
                         CycleESAux(s->rest(), s0), T);
  }
}

template <typename T>
EStream <T> *DropES(EStream<T> *s0, int n) {
  if (n == 0) {
    return s0;
  } else {
    return DropES(s0->rest(), n-1);
  }
}

EStream<int> *IntegersES(int starting) {
  return CREATE_STREAM(starting,
                       IntegersES(starting + 1),
                       int);
  // return new EStream<int>(starting,
  //                         [=]()->EStream<int>*{ IntegersES(starting + 1); });
}

template <typename T>
EStream<T> *InterleaveES(EStream<T> *s0, EStream<T> *s1) {
  if (s0 == NULL_ESTREAM(T)) {
    return s1;
  } else if (s1 == NULL_ESTREAM(T)) {
    return s0;
  } else {
    return CREATE_STREAM(s0->first(),
                         InterleaveES(s1, s0->rest()),
                         T);
  }
}

template <typename T>
EStream<T> *TakeES(EStream<T> *s, int n) {
  if ((s == NULL_ESTREAM(T)) || (n <= 0)) {
    return NULL_ESTREAM(T);
  } else {
    return CREATE_STREAM(s->first(),
                         TakeES(s->rest(), n-1),
                         T);
  }
}


// ================================================================  
// ================================================================  

// Test out the various streams...
void test(EStream<int> *s,
          std::string name,
          std::string expected,
          int max = -1) {
  printf("=== %s expect %s\n", name.c_str(), expected.c_str());
  printf("    %*c got    ", (int)name.size(), ' ');
  for (int i=max; (i != 0) && (s != NULL_ESTREAM(int)); i--, s = s->rest()) {
    printf("%d ", s->first());
  }
  printf("\n");
}

int main( void ) {

  EStream<int> *append = AppendES(TakeES(IntegersES(10), 3),
                                  TakeES(IntegersES(20), 3));
  test(append, "AppendES", "10 11 12 20 21 22");

  EStream<int> *constant = ConstantES(42);
  test(constant, "ConstantES", "42 42 42 42 42", 5);

  EStream<int> *cycle = CycleES(TakeES(IntegersES(0), 3));
  test(cycle, "CycleES", "0 1 2 0 1 2 0 1 2 0 1 2", 12);

  EStream<int> *drop = DropES(IntegersES(0), 4);
  test(drop, "DropES", "4 5 6 7 8", 5);

  EStream<int> *ints = IntegersES(22);
  test(ints, "IntegersES", "22 23 24 25 26", 5);

  EStream<int> *interleave = InterleaveES(TakeES(IntegersES(10), 3),
                                          TakeES(IntegersES(20), 3));
  test(interleave, "InterleaveES", "10 20 11 21 12 22");
    
  EStream<int> *take = TakeES(IntegersES(0), 5);
  test(take, "TakeES", "0 1 2 3 4");

  return 0;
}
