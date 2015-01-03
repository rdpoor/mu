/*
 * Making a fluent interface with a base class
 * see http://stackoverflow.com/questions/2768282/how-to-implement-fluent-interface-with-a-base-class-in-c
 */
#include <iostream>

template <typename T> class Base {
public:
  Base() : x_ ( 0 ) { }
  T& setX(int x) { x_ = x; return *static_cast<T *>(this); }
protected:
  int x_;
};
  
class A : public Base<A> {
public:
  A& setY(int y) { y_ = y; return *this; }
protected:
  int y_;
};

class B : public Base<B> {
public:
  B& setZ(int z) { z_ = z; return *this; }
protected:
  int z_;
};

int main(void) {
  A a;
  B b;
  a.setX(1).setY(2);
  b.setX(3).setZ(4);

  return 0;
}
