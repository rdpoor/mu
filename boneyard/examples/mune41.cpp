/*
 * Test a class with its own pool / factory
 */
#include "mu_pool.h"
#include <printf.h>

// ================================================================
// sample

class Widget {
  // Give MuPool access to protected constructor.
  friend mu::MuPool<Widget> Widget();

public:
  static Widget *allocate() {
    return s_pool_.allocate();
  }

  static void deallocate(Widget *widget) {
    s_pool_.deallocate(widget);
  }

  int value() { return value_;  }

  Widget() : value_(-1) {
    printf("allocating %p\n", this);
  }

protected:
  ~Widget() {
    printf("deallocating %p\n", this);
  }
  // This next line gives linker error:
  // Undefined symbols for architecture x86_64: "Widget::s_pool_"
  static mu::MuPool<Widget> s_pool_;
  int value_;
};

// ================================================================
//

int main() {

  Widget *w1 = Widget::allocate();
  Widget *w2 = Widget::allocate();

  Widget::deallocate(w1);
  Widget::deallocate(w2);
}
