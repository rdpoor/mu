/*
 * side-effect free streams.  uses create() to create a new stream
 * with a protected constructor.  ToDo:
 * - figure out when to call delete() on objects we've created
 * - support variable # of stream arguments.
 */

// #include <stdio.h>
// #include <vector>
#include <string>

#define NULL_ESTREAM(T) ((EStream<T> *)0)

// (Abstract) Base Class
template <typename T> class EStream {
public:
  virtual T first() = 0;
  virtual EStream<T> *rest() = 0;
};

// ================================================================
// alphabetical below here

// ================
// Return elements of s0 followed by elements of s1
template <typename T> class AppendES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s0, EStream <T> *s1 ) {
    return (s0 == NULL_ESTREAM(T)) ? s1 : new AppendES<T>(s0, s1);
  }

  T first() {
    return s0_->first(); 
  }

  EStream<T> *rest() { 
    return AppendES::create(s0_->rest(), s1_);
  }

protected:
  AppendES( EStream<T> *s0, EStream<T> *s1) : s0_(s0), s1_(s1) {}
  EStream<T> *s0_;
  EStream<T> *s1_;
};

// ================
// Generate a stream of constant values
template <typename T> class ConstantES : public EStream <T> {
public:
  static ConstantES<T> *create(T value) { return new ConstantES<T>(value); }
  T first() { return value_; }
  ConstantES<T> *rest() { return this; }
protected:
  ConstantES(T value): value_ (value) {};
  T value_;
};

// ================
// Restart stream s when it is exhausted
template <typename T> class CycleES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s ) {
    return (s == NULL_ESTREAM(T)) ? s : new CycleES(s, s);
  }

  T first() {
    return s_->first(); 
  }

  EStream<T> *rest() { 
    EStream<T> *t = s_->rest();
    if (t == NULL_ESTREAM(T)) {
      return new CycleES(s_head_, s_head_);
    } else {
      return new CycleES(t, s_head_);
    }
  }

protected:
  CycleES( EStream<T> *s, EStream<T> *s_head) : s_(s), s_head_(s_head) {}
  EStream<T> *s_;
  EStream<T> *s_head_;
};

// ================
// Drop the first n elements of a stream
template <typename T> class DropES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s, int n) {
    while ((n > 0) && (s != NULL_ESTREAM(T))) {
      n -= 1;
      s = s->rest();
    }
    return s;
  }

  // these should never get called.
  T first() { return (T)0; }
  EStream<T> *rest() { return NULL_ESTREAM(T); }

protected:
  DropES() {}
};

// ================
// Generate a stream of integers, counting up from a starting value
class IntegerES : public EStream<int> {
public:
  static IntegerES *create(int initial) { return new IntegerES(initial); }
  int first() { return value_; }
  IntegerES *rest() { return IntegerES::create(value_ + 1); }
protected:
  IntegerES(int initial): value_ (initial) {}
  int value_;
};

// ================
// Return elements of s0, then s1, then s0.  If either stream is
// empty, emit the other stream.
template <typename T> class InterleaveES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s0, EStream <T> *s1 ) {
    return (s0 == NULL_ESTREAM(T)) ? s1 : new InterleaveES<T>(s0, s1);
  }

  T first() {
    return s0_->first(); 
  }

  EStream<T> *rest() { 
    return InterleaveES::create(s1_, s0_->rest());
  }

protected:
  InterleaveES( EStream<T> *s0, EStream<T> *s1) : s0_(s0), s1_(s1) {}
  EStream<T> *s0_;
  EStream<T> *s1_;
};

// ================
// Return the first n elements of a stream
template <typename T> class TakeES : public EStream <T> {
public:
  static EStream<T> *create(EStream<T> *s, int n) {
    return ((n <= 0) || (s == NULL_ESTREAM(T))) ? NULL_ESTREAM(T) : new TakeES<T>(s, n);
  }
  T first() { 
      return s_->first();
  }
  EStream<T> *rest() {
    return TakeES::create(s_->rest(), n_ - 1);
  }
protected:
  TakeES(EStream<T> *s, int n): s_ (s), n_ (n) {}

  EStream<T> *s_;
  int n_;
};

// ================================================================  
// ================================================================  

// Test out the various streams...
void test(EStream<int> *s, std::string name, std::string expected, int max = -1) {
  printf("=== %s expect %s\n", name.c_str(), expected.c_str());
  printf("    %*c got    ", (int)name.size(), ' ');
  for (int i=max; (i != 0) && (s != NULL_ESTREAM(int)); i--, s = s->rest()) {
    printf("%d ", s->first());
  }
  printf("\n");
}

int main( void ) {

  EStream<int> *append= AppendES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                              TakeES<int>::create(IntegerES::create(20), 3));
  test(append, "AppendES", "10 11 12 20 21 22");

  EStream<int> *constant = ConstantES<int>::create(42);
  test(constant, "ConstantES", "42 42 42 42 42", 5);

  EStream<int> *cycle = CycleES<int>::create(TakeES<int>::create(IntegerES::create(0), 3));
  test(cycle, "CycleES", "0 1 2 0 1 2 0 1 2 0 1 2", 12);

  EStream<int> *drop = DropES<int>::create(IntegerES::create(0), 4);
  test(drop, "DropES", "4 5 6 7 8", 5);

  EStream<int> *ints = IntegerES::create(22);
  test(ints, "IntegerES", "22 23 24 25 26", 5);

  EStream<int> *interleave = InterleaveES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                                       TakeES<int>::create(IntegerES::create(20), 3));
  test(interleave, "InterleaveES", "10 20 11 21 12 22");
    
  EStream<int> *take = TakeES<int>::create(IntegerES::create(0), 5);
  test(take, "TakeES", "0 1 2 3 4");

  return 0;
}
