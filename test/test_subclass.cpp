#include <stdio.h>

class A {
public:
  void doIt() {
    printf("doIt in A\n");
  }
  void shadowed() {
    printf("shadowed in A\n");
  }

};

class B : public A {
public:
  void shadowed() {
    printf("shadowed in B\n");
  }

};

void testIt(A& obj) {
  obj.doIt();
  obj.shadowed();
}

int main() {
  A a;
  B b;
  testIt(a);
  testIt(b);
  return 0;
}
