#include <stdio.h>

void foo(int a[], int len) {
  for (int i=0; i<len; i++) { printf("%d ", a[i]); }
  printf("\n");
}

int main() {
  foo((int []){1, 2, 3}, 3);
  foo((int []){4, 5, 6, 7}, 4);
  
  return 0;
}
