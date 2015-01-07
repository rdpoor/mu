#include <cstdlib>
#include <iostream>

class A {
public:
  virtual void f() = 0;
  virtual void g();
  virtual void h();
};

void A::f() {
  std::cout << "A::f()" << std::endl;
}

void A::g() {
  std::cout << "A::g()" << std::endl;
}

void A::h() {
  std::cout << "A::h()" << std::endl;
}

class B : public A {
public:
  // B *must* define f() since it's virtual and pure in A
  virtual void f() {
    A::f();
  }
  // B *may* define g() since it's virtual in A
  virtual void g() {
    A::g();
  }
  // B may skip defining h() to inherit from A
};

int main() {
  // A a; -- cannot instantiate since A is virtual
  B b;

  b.f();
  b.g();
  b.h();

  return 0;
}
