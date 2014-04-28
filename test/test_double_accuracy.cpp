/*
 * If we use (double) seconds to represent time, at what point can 
 * seconds no longer reprepresent a single frame increment?
 *
 * Answer: about 102122440529 seconds.  Plenty good enough.
 */
#include <stdio.h>

#define SRATE 44100.0

long int s_to_f(double s) { return (long int)(s * SRATE); }
double f_to_s(long int f) { return (double)(f / SRATE); }

bool test(long int f0) {
  double s0 = f_to_s(f0);
  double s1 = s0 + (1.0 / SRATE);
  long int f1 = f0 + 1;
  long int fh = s_to_f(s1);
  printf("%20ld %20ld %20f %20f\n", f1, fh, s0, s1);
  return f1 == fh;
}

int main() {
  long int f = 1;
  while (true) {
    if (!test(f)) break;
    // printf("%ld, %f\n", f, f_to_s(f));
    f = f * 2;
    if (f < 0) break;
  }
  return 0;
}
