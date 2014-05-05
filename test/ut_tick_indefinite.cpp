// Test primitive operations on the Tick data type

#include "assert.h"
#include "mu.h"

int main() {
  mu::TickIndefinite i;
  long int n = -1;
  long int z = 0;
  long int p = 1;

  ASSERT(i == mu::TickIndefinite::kIndefinite);

  ASSERT((mu::TickIndefinite::kIndefinite + mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite + n) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite + z) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite + p) == mu::TickIndefinite::kIndefinite);

  ASSERT((n + mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((z + mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((p + mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);

  ASSERT((mu::TickIndefinite::kIndefinite - mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite - n) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite - z) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite - p) == mu::TickIndefinite::kIndefinite);

  ASSERT((n - mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((z - mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((p - mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);

  ASSERT((mu::TickIndefinite::kIndefinite * mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite * n) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite * z) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite * p) == mu::TickIndefinite::kIndefinite);

  ASSERT((n * mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((z * mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((p * mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);

  ASSERT((mu::TickIndefinite::kIndefinite / mu::TickIndefinite::kIndefinite) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite / n) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite / z) == mu::TickIndefinite::kIndefinite);
  ASSERT((mu::TickIndefinite::kIndefinite / p) == mu::TickIndefinite::kIndefinite);

  ASSERT((n / mu::TickIndefinite::kIndefinite) == z);
  ASSERT((z / mu::TickIndefinite::kIndefinite) == z);
  ASSERT((p / mu::TickIndefinite::kIndefinite) == z);

  ASSERT((mu::TickIndefinite::kIndefinite > mu::TickIndefinite::kIndefinite) == false);
  ASSERT((mu::TickIndefinite::kIndefinite > n) == true);
  ASSERT((mu::TickIndefinite::kIndefinite > z) == true);
  ASSERT((mu::TickIndefinite::kIndefinite > p) == true);

  ASSERT((mu::TickIndefinite::kIndefinite >= mu::TickIndefinite::kIndefinite) == true);
  ASSERT((mu::TickIndefinite::kIndefinite >= n) == true);
  ASSERT((mu::TickIndefinite::kIndefinite >= z) == true);
  ASSERT((mu::TickIndefinite::kIndefinite >= p) == true);

  ASSERT((mu::TickIndefinite::kIndefinite == mu::TickIndefinite::kIndefinite) == true);
  ASSERT((mu::TickIndefinite::kIndefinite == n) == false);
  ASSERT((mu::TickIndefinite::kIndefinite == z) == false);
  ASSERT((mu::TickIndefinite::kIndefinite == p) == false);

  ASSERT((mu::TickIndefinite::kIndefinite <= mu::TickIndefinite::kIndefinite) == true);
  ASSERT((mu::TickIndefinite::kIndefinite <= n) == false);
  ASSERT((mu::TickIndefinite::kIndefinite <= z) == false);
  ASSERT((mu::TickIndefinite::kIndefinite <= p) == false);

  ASSERT((mu::TickIndefinite::kIndefinite < mu::TickIndefinite::kIndefinite) == false);
  ASSERT((mu::TickIndefinite::kIndefinite < n) == false);
  ASSERT((mu::TickIndefinite::kIndefinite < z) == false);
  ASSERT((mu::TickIndefinite::kIndefinite < p) == false);

  return 0;
}
