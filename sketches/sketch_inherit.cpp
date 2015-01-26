// How can I call methods defined in intermediate subclasses?

#include <stdio.h>

class A {
protected:
  void general( void ) {
    printf("general in a\n");
  }
  void specific_to_a( void ) {
    printf("specific to a\n");
  }
};

class B : public A {
protected:
  void general( void ) {
    printf("general in b\n");
  }
  void specific_to_b( void ) {
    printf("specific to b\n");
  }
};

class C : public B {
public:
  void general( void ) {
    A::general();
    B::general();
  }

  void specific( void ) {
    specific_to_a();
    specific_to_b();
  }
};

int main() {
  C c;
  c.general();
  c.specific();
  return 0;
}
