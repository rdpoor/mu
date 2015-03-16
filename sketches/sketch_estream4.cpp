/*
 * streams with side-effects (contrast with sketch_estream3): calling
 * rest() alters internal state and (usually) returns this or
 * NULL_ESTREAM.  The constructor is protected and instead a static
 * create() method is provided so it can return NULL_ESTREAM as
 * needed.  A copy() method creates a deep copy of the stream,
 * allowing a snapshot of the current state to be preserved.
 */

#include <stdio.h>
#include <vector>
#include <string>

#define NULL_ESTREAM(T) ((EStream<T> *)0)

// (Abstract) Base Class
template <typename T> class EStream {
public:
  virtual EStream<T> *copy() = 0;
  virtual T first() = 0;
  virtual EStream<T> *rest() = 0;
};

// ================================================================
// prototypical one input stream
template <typename T> class IdentityES : public EStream <T> {
public:
  static IdentityES<T> *create( EStream <T> *source) {
    return (source == NULL_ESTREAM(T)) ? (IdentityES<T> *)0 : new IdentityES<T>(source);
  }

  IdentityES<T> *copy( void ) {
    return new IdentityES<T>(source_->copy());
  }

  T first() {
    return source_->first();
  }

  IdentityES<T> *rest( void ) {
    return ((source_ = source_->rest()) == NULL_ESTREAM(T)) ? (IdentityES<T> *)0 : this;
  }

protected:
  IdentityES( EStream<T> *source) : source_(source) {}

  EStream<T> *source_;
};
  
// ================
// Return elements of s0 followed by elements of s1
template <typename T> class AppendES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s0, EStream <T> *s1 ) {
    if (s0 == NULL_ESTREAM(T)) {
      return s1;
    } else if (s1 == NULL_ESTREAM(T)) {
      return s0;
    } else {
      return new AppendES(s0, s1);
    }
  }

  EStream<T> *copy( void ) {
    return new AppendES<T>(s0_->copy(), s1_->copy());
  }

  T first() {
    return s0_->first(); 
  }

  EStream<T> *rest() { 
    s0_ = s0_->rest();
    return (s0_ == NULL_ESTREAM(T)) ? s1_ : this;
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
  static ConstantES<T> *create( T value ) {
    return new ConstantES(value);
  }

  ConstantES<T> *copy( void ) {
    return this;
  }

  T first() { return value_; }

  ConstantES<T> *rest() { return this; }

protected:
  ConstantES(T value): value_ (value) {}

  T value_;
};

// ================
// Restart stream s when it is exhausted
template <typename T> class CycleES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s ) {
    return (s == NULL_ESTREAM(T)) ? NULL_ESTREAM(T) : new CycleES(s, s->copy());
  }

  EStream<T> *copy( void ) {
    return new CycleES(s_->copy(), s1_->copy());
  }

  T first() {
    return s1_->first(); 
  }

  EStream<T> *rest() { 
    s1_ = s1_->rest();
    if (s1_ == NULL_ESTREAM(T)) {
      s1_ = s_->copy();
    }
    return this;
  }

protected:
  CycleES( EStream<T> *s, EStream<T> *s1) : s_(s), s1_(s1) {}

  EStream<T> *s_;
  EStream<T> *s1_;
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
  EStream<T> *copy() { return NULL_ESTREAM(T); }
  T first() { return (T)0; }
  EStream<T> *rest() { return NULL_ESTREAM(T); }

protected:
  DropES() {}
};

// ================
// Generate a stream of integers, counting up from a starting value
class IntegerES : public EStream<int> {
public:
  static IntegerES *create(int initial) { 
    return new IntegerES(initial); 
  }

  IntegerES *copy( void ) {
    return new IntegerES(value_);
  }

  int first() { 
    return value_; 
  }

  IntegerES *rest() { 
    value_ += 1;
    return this;
  }

protected:
  IntegerES(int initial): value_(initial) {}
  int value_;
};

// ================
// Return elements of s0, then s1, then s0.  If either stream is
// empty, emit the other stream.
template <typename T> class InterleaveES : public EStream <T> {
public:

  static EStream<T> *create( EStream <T> *s0, EStream <T> *s1 ) {
    if (s0 == NULL_ESTREAM(T)) {
      return s1;
    } else if (s1 == NULL_ESTREAM(T)) {
      return s0;
    } else {
      return new InterleaveES<T>(s0, s1);
    }
  }

  EStream<T> *copy( void ) {
    return new InterleaveES(s0_->copy(), s1_->copy());
  }

  T first() {
    return s0_->first(); 
  }

  EStream<T> *rest() { 
    s0_ = s0_->rest();
    if (s0_ == NULL_ESTREAM(T)) {
      return s1_;
    } else {
      EStream<T> *t = s0_;
      s0_ = s1_;
      s1_ = t;
      return this;
    }
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

  EStream<T> *copy( void ) {
    return new TakeES<T>(s_->copy(), n_);
  }

  T first() { 
      return s_->first();
  }

  EStream<T> *rest() {
    n_ = n_ - 1;
    s_ = s_->rest();
    return ((n_ <= 0) || (s_ == NULL_ESTREAM(T))) ? NULL_ESTREAM(T) : this;
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
  printf("=== %s\n", name.c_str());
  printf("expect %s\n   got ", expected.c_str());
  for (int i=max; (i != 0) && (s != NULL_ESTREAM(int)); i--, s = s->rest()) {
    printf("%d ", s->first());
  }
  printf("\n");
}

int main( void ) {

  IdentityES<int> *identity = IdentityES<int>::create(IntegerES::create(22));
  IdentityES<int> *identity2 = identity->copy();
  test(identity, "IdentityES", "22 23 24 25 26", 5);
  test(identity2, "IdentityES copy", "22 23 24 25 26 27", 6);
  IdentityES<int> *identity3 = IdentityES<int>::create(NULL_ESTREAM(int));
  test(identity3, "Identity3", "");

  EStream<int> *append = AppendES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                               TakeES<int>::create(IntegerES::create(20), 3));
  EStream<int> *append2 = append->copy();
  test(append, "AppendES", "10 11 12 20 21 22");
  test(append2, "AppendES copy", "10 11 12 20 21 22");
  EStream<int> *append3 = AppendES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                                NULL_ESTREAM(int));
  test(append3, "Append3", "10 11 12");
  EStream<int> *append4 = AppendES<int>::create(NULL_ESTREAM(int),
                                                TakeES<int>::create(IntegerES::create(20), 3));
  test(append4, "Append4", "20 21 22");
  EStream<int> *append5 = AppendES<int>::create(NULL_ESTREAM(int), NULL_ESTREAM(int));
  test(append5, "Append5", "");

  EStream<int> *constant = ConstantES<int>::create(42);
  EStream<int> *constant2 = constant->copy();
  test(constant, "ConstantES", "42 42 42 42 42", 5);
  test(constant2, "ConstantES copy", "42 42 42 42 42", 5);

  EStream<int> *cycle = CycleES<int>::create(TakeES<int>::create(IntegerES::create(0), 3));
  EStream<int> *cycle2 = cycle->copy();
  test(cycle, "CycleES", "0 1 2 0 1 2 0 1 2 0 1 2", 12);
  test(cycle2, "CycleES copy", "0 1 2 0 1 2 0 1 2 0 1 2", 12);
  EStream<int> *cycle3 = CycleES<int>::create(NULL_ESTREAM(int));
  test(cycle3, "Cycle3", "");

  EStream<int> *drop = DropES<int>::create(IntegerES::create(0), 4);
  EStream<int> *drop2 = drop->copy();
  test(drop, "DropES", "4 5 6 7 8", 5);
  test(drop2, "DropES copy", "4 5 6 7 8", 5);
  EStream<int> *drop3 = DropES<int>::create(NULL_ESTREAM(int), 4);
  test(drop3, "Drop3", "");

  EStream<int> *ints = IntegerES::create(22);
  EStream<int> *ints2 = ints->copy();
  test(ints, "IntegerES", "22 23 24 25 26", 5);
  test(ints2, "IntegerES copy", "22 23 24 25 26", 5);

  EStream<int> *interleave = InterleaveES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                                       TakeES<int>::create(IntegerES::create(20), 3));
  EStream<int> *interleave2 = interleave->copy();
  test(interleave, "InterleaveES", "10 20 11 21 12 22");
  test(interleave2, "InterleaveES copy", "10 20 11 21 12 22");
  EStream<int> *interleave3 = InterleaveES<int>::create(TakeES<int>::create(IntegerES::create(10), 3),
                                                        NULL_ESTREAM(int));
  test(interleave3, "InterleaveES3", "10 11 12");
  EStream<int> *interleave4 = InterleaveES<int>::create(NULL_ESTREAM(int),
                                                       TakeES<int>::create(IntegerES::create(20), 3));
  test(interleave4, "InterleaveES4", "20 21 22");
  EStream<int> *interleave5 = InterleaveES<int>::create(NULL_ESTREAM(int), NULL_ESTREAM(int));
  test(interleave5, "InterleaveES5", "");
    
  EStream<int> *take = TakeES<int>::create(IntegerES::create(0), 5);
  EStream<int> *take2 = take->copy();
  test(take, "TakeES", "0 1 2 3 4");
  test(take2, "TakeES copy", "0 1 2 3 4");
  EStream<int> *take3 = TakeES<int>::create(NULL_ESTREAM(int), 5);
  test(take3, "TakeES3", "");


  return 0;
}
