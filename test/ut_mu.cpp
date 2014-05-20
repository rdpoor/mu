/*
 * Unit Test Mu utilities
 */
#include "assert.h"
#include "mu.h"
#include <unistd.h>

int main() {

  ASSERT(near(1.0, mu::dbToValue(0)));
  ASSERT(near(10.0, mu::dbToValue(20)));
  ASSERT(near(0.1, mu::dbToValue(-20)));

}
