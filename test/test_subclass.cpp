#include <stdio.h>

class A {
public:
  virtual int x() { return 42; }
  int double_x()  { return 2 * this->x(); }
};

class B : public A {
public:
  int x() { return 1; };
};

int main() {
  A a;
  B b;
  printf("a.x() = %d (expect 42)\n", a.x());
  printf("a.double_x() = %d (expect 84)\n", a.double_x());
  printf("b.x() = %d (expect 1)\n", b.x());
  printf("b.double_x() = %d (expect 2)\n", b.double_x());
  return 0;
}
