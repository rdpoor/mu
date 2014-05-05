#include <map>
#include <stdio.h>
#include <unistd.h>
#include "mu.h"

typedef std::map<mu::Tick, stk::StkFloat> Breakpoints;

Breakpoints breakpoints;
Breakpoints::iterator itlo, ithi;

void test(mu::Tick tick) {
  itlo = breakpoints.lower_bound(tick);
  ithi = breakpoints.upper_bound(tick);
  bool before_start = (tick < breakpoints.begin()->first);
  bool at_start = (tick == breakpoints.begin()->first);
  bool at_end = (itlo != breakpoints.end()) && (ithi == breakpoints.end());
  bool after_end = (itlo == breakpoints.end()) && (ithi == breakpoints.end());
  fprintf(stderr,"=== tick = %ld, before_start=%d, at_start=%d, at_end=%d, after_end=%d\n", 
          tick,
          before_start,
          at_start,
          at_end,
          after_end);
}

int main() {

  
  breakpoints[10] = 10.0;
  breakpoints[20] = 11.0;
  breakpoints[120] = 21.0;

  test(1);
  test(10);
  test(15);
  test(25);
  test(119);
  test(120);
  test(121);

  return 0;
}

  
