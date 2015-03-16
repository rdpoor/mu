/*
 * can a typedef and a class have the same name?
 * Answer: nope.
 */

#include <stdio.h>
#include <limits.h>

typedef long int Foo;

class FooUtils {
public:
  static Foo indefinite() { return LONG_MAX; }
};

int main() {
  Foo a;

  a = FooUtils::indefinite();
  printf("a = %ld\n", a);

  return 0;
}
