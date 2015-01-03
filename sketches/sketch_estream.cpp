/*
 * some possible streams
 */

#include <stdio.h>

template <typename T> class EStream {
public:
  virtual T first();
  virtual void next();
};

template <typename T> class ConstantES : public EStream <T> {
public:
  ConstantES(T value): value_ (value) {}
  T first() { return value_; }
  void next() { }
protected:
  T value_;
};

class IntegerES : public EStream<int> {
public:
  IntegerES(int initial): value_ (initial) {}
  IntegerES( IntegerES& es ) : value_ (es.first()) {}
  int first() { return value_; }
  void next() { value_ += 1; }
protected:
  int value_;
};

template <typename T> class KeepES : public EStream <T> {
public:
  KeepES(EStream<T> *s, int n): s_ (s), n_ (n) {}
  KeepES<T>( KeepES<T> &es ): n_ (es.n_) { s_ = es.s_; }
  T first() { return (n_ <= 0) ? (T)0 : s_->first(); }
  void next() { 
    n_ -= 1;
    s_->next();
  }
protected:
  EStream<T> *s_;
  int n_;
};

int main( void ) {
  EStream<int> *a = new IntegerES(22);
  KeepES<int> *c = new KeepES<int>(a, 4);
  KeepES<int> d = *c;

  for (int i=0; i<5; i++) {
    printf("%d %p\n", a->first(), a);
    a->next();
  }

  printf("\n");
  while (c->first()) {
    printf("%d %p\n", c->first(), c);
    c->next();
  }

  // In this sketch, stream elements are non-constant: they change their
  // internal state in a call to next().  The copy operator is supposed
  // to make a snapshot of the state so you can use the copy to get back
  // to that previous state.  
  //
  // The following exposes a brittleness to this design: If the copy
  // operator fails to snapshot the state of all inputs (transitively),
  // then you aren't restored to the previous state.  In this case,
  // KeepES d is reading from the original stream a, and a has already
  // been incremented.
  printf("\n");
  while (d.first()) {
    printf("%d %p\n", d.first(), &d);
    d.next();
  }
  
  return 0;
}
