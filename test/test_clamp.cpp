#include "mu.h"
#include <stdio.h>

void assert(bool b, const char *msg) {
  printf("%s: %s\n", msg, (b?"true":"false"));
}

int main() {
  long int i;
  double j;

  assert(mu::clamp(-10, -20, 10) == -10, "mu::clamp(-10, -20, 10) == -10");
  assert(mu::clamp(-10, -1, 10) == -1, "mu::clamp(-10, -1, 10) == -1");
  assert(mu::clamp(-10, 20, 10) == 10, "mu::clamp(-10, 20, 10) == 10");

  assert(mu::clamp(-10.0, -20.0, 10.0) == -10.0, "mu::clamp(-10.0, -20.0, 10.0) == -10.0");
  assert(mu::clamp(-10.0, -1.0, 10.0) == -1.0, "mu::clamp(-10.0, -1.0, 10.0) == -1.0");
  assert(mu::clamp(-10.0, 20.0, 10.0) == 10.0, "mu::clamp(-10.0, 20.0, 10.0) == 10.0");

  i=-20; assert(mu::clamp(-10l, i, 10l) == -10, "i=-20; mu::clamp(-10, i, 10) == -10");
  i=-1; assert(mu::clamp(-10l, i, 10l) == -1, "i=-1; mu::clamp(-10, i, 10) == -1");
  i=20; assert(mu::clamp(-10l, i, 10l) == 10, "i=20; mu::clamp(-10, i, 10) == 10");

  j=-20.0; assert(mu::clamp(-10.0, j, 10.0) == -10.0, "j=-20.0; mu::clamp(-10.0, j, 10.0) == -10.0");
  j=-1.0; assert(mu::clamp(-10.0, j, 10.0) == -1.0, "j=-1.0; mu::clamp(-10.0, j, 10.0) == -1.0");
  j=20.0; assert(mu::clamp(-10.0, j, 10.0) == 10.0, "j=20.0; mu::clamp(-10.0, j, 10.0) == 10.0");

}
