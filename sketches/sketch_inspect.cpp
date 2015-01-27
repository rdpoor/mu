// Does calling intermediate method automatically delegate to the base method?

#include <stdio.h>

class Base {
public:
  virtual void doit() {
    printf("Base doit()\n");
  }
};

class Intermediate : public Base {
public:
};

class Leaf : public Intermediate{
public:
  virtual void doit() {
    Intermediate::doit();
    printf("Leaf doit()\n");
  }
};


int main() {
  Leaf x;

  x.doit();

  return 0;
}
