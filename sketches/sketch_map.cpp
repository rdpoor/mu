#include <map>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

typedef std::map<long int, double> Breakpoints;

Breakpoints breakpoints;
Breakpoints::iterator itlo, ithi;

void test(long int tick) {
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

void test2(long int tick) {
  itlo = breakpoints.lower_bound(tick);
  ithi = breakpoints.upper_bound(tick);
  std::cout << "=== itlo(" << tick << "): " << itlo->first << ", " << itlo->second << '\n';
  std::cout << "    ithi(" << tick << "): " << ithi->first << ", " << ithi->second << '\n';
  if (ithi != breakpoints.begin()) {
    Breakpoints::iterator it = ithi;
    it--;
    std::cout << "    ithi-1(" << tick << "):" << it->first << ", " << it->second << "\n";
  }
}

void bound(long int tick) {
  Breakpoints::iterator it, lo, hi;
  it = breakpoints.upper_bound(tick);
  hi = it;
  lo = it;
  if (it != breakpoints.begin()) {
    lo--;
    std::cout << "bound(" << tick << "): [[" << lo->first << "," << lo->second << "],[" << hi->first << "," << hi->second << "]]\n";
  } else {
    std::cout << "bound(" << tick << "): [[" << lo->first << "," << lo->second << "],[" << hi->first << "," << hi->second << "]]\n";
  }    
}

void test3() {
  Breakpoints b;
  Breakpoints::iterator it;
  it = b.begin();
  // if (it != NULL) {
  printf("on empty map it = %p, it->first = %ld, it->second = %f\n", &it, it->first, it->second);
}


  
int main() {

  test3();

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

  bound(1);
  bound(10);
  bound(15);
  bound(25);
  bound(119);
  bound(120);
  bound(121);

  
  return 0;
}

  
