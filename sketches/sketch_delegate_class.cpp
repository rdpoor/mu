#include <iostream>

class Foo {
public:
  Foo( void ) 
    : iv_ (22) {
  }

  void p() {
    std::cout << iv_;
  }

protected:
  int iv_;
};

class Bar: public Foo {
};

int main() {
  Bar *bar = new Bar();
  bar->p();
}

