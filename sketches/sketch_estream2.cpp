/*
 * side-effect free streams.
 */

#include <stdio.h>
#include <vector>
#include <string>

#define NULL_ESTREAM(T) ((EStream<T> *)0)

template <typename T> class EStream {
public:
  virtual T first() = 0;
  virtual EStream<T> *rest() = 0;
};

template <typename T> class ConstantES : public EStream <T> {
public:
  ConstantES(T value): value_ (value) {}
  T first() { return value_; }
  ConstantES<T> *rest() { return new ConstantES(value_); }
protected:
  T value_;
};

template <typename T> class AppendES : public EStream <T> {
public:
  AppendES( std::vector<EStream <T> *> streams ) : streams_ (streams) {
    printf("streams = %p, streams_ = %p\n", &streams, &streams_);
  }
  T first() {return (T)0; }
  AppendES<T> *rest() { return (AppendES<T> *)0; }
protected:
    std::vector<EStream <T> *> streams_;
};

class IntegerES : public EStream<int> {
public:
  IntegerES(int initial): value_ (initial) {}
  int first() { return value_; }
  IntegerES *rest() { return new IntegerES(value_ + 1); }
protected:
  int value_;
};

template <typename T> class KeepES : public EStream <T> {
public:
  KeepES(EStream<T> *s, int n): s_ (s), n_ (n) {}
  T first() { 
    if ((n_ <= 0) || (s_ == NULL_ESTREAM(T))) {
      return (T)0;
    } else {
      return s_->first();
    }
  }
  KeepES<T> *rest() {
    if ((n_ <= 0) || (s_ == NULL_ESTREAM(T))) {
      return (KeepES<T> *)0;
    } else {
      EStream<T> *t = s_->rest();
      return new KeepES(t, n_ - 1);
    }
  }
protected:
  EStream<T> *s_;
  int n_;
};

// ================================================================  
// ================================================================  

int main( void ) {
  EStream<int> *a = new IntegerES(22);
  KeepES<int> *c = new KeepES<int>(a, 4);
  KeepES<int> *d = c;

  for (int i=0; i<5; i++) {
    printf("%d %p\n", a->first(), a);
    a = a->rest();
  }

  printf("\n");
  while (c->first()) {
    printf("%d %p\n", c->first(), c);
    c = c->rest();
  }

  printf("\n");
  while (d->first()) {
    printf("%d %p\n", d->first(), d);
    d = d->rest();
  }
  
  printf("\n");
  EStream<int> *arr3[] = {new IntegerES(10), new IntegerES(20), new IntegerES(30)};
  std::vector< EStream<int> *> v3( arr3, arr3+3 );
  
  AppendES<int> *e = new AppendES<int>(v3);
  while (e->first()) {
    printf("%d %p\n", e->first(), e);
    e = e->rest();
  }
    
  return 0;
}
