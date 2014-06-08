/*
 * Adventures in initializing and sorting a 3-element struct.
 * Probably really (really) fast: no malloc, no setup.
 */
#include <stdio.h>
#include <unistd.h>

typedef struct {
  double v;
  int i;
} pair;

void swap_pairs(pair &a, pair &b) {
  double tmp_v = a.v;
  int tmp_i = a.i;
  a.v = b.v;
  a.i = b.i;
  b.v = tmp_v;
  b.i = tmp_i;
}

void sort_pairs(pair a[]) {
  if (a[0].v > a[1].v) swap_pairs(a[0], a[1]);
  if (a[1].v > a[2].v) swap_pairs(a[1], a[2]);
  if (a[0].v > a[1].v) swap_pairs(a[0], a[1]);
}

void print_pairs(pair a[]) {
  for (int i=0; i<3; i++) {
    printf("[%f %d] ", a[i].v, a[i].i);
  }
  printf("\n");
}

int main() {

  int x0 = 22;
  double y0 = 2.1;

  pair pairs[3] = {{2.0, 10}, {y0, x0}, {1.9, 30}};

  print_pairs(pairs);
  sort_pairs(pairs);
  print_pairs(pairs);

  return 0;
}
