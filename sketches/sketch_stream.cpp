// Nice treatment on templates:
// http://stackoverflow.com/questions/2079296/c-templates-linkedlist

#include <stdio.h>

template <typename T> class Stream {
public:
  virtual const T first() const;
  virtual const Stream<T> *rest() const;
};

class Ones : public Stream<int> {
public:
  const int first() const { return 1; }
  const Ones *rest() const { return this; }
};

class Integers : public Stream<int> {
public:
  Integers(const int initial) : first_ (initial) { }
  const int first() const { return first_; }
  const Integers *rest() const { return new Integers(first_ + 1); }
protected:
  const int first_;
};

class Sum : public Stream<int> {
public:
  Sum(const Stream<int> *a, const Stream<int> *b) :
    a_ (a),
    b_ (b) {
  }
  const int first() const { return a_->first() + b_->first(); }
  const Sum *rest() const { return new Sum(a_->rest(), b_->rest()); }
protected:
  const Stream<int> *a_;
  const Stream<int> *b_;
};
    
// ================================================================
// ================================================================

int main( void ) {
  const Stream<int> *a = new Ones();
  const Stream<int> *b = new Integers(100);
  const Stream<int> *c = new Sum(a, b);

  for (int i=0; i<10; i++) {
    printf("%d %p\n", c->first(), c);
    c = c->rest();
  }
  return 0;
}

