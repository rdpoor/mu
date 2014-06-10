// #include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include "mu.h"

// can NULL be passed as a string?

void foo(const char* s) {
  if (s == NULL) {
    std::cout << "null string" << std::endl;
  } else {
    std::cout << s << std::endl;
  }
}

void bar(std::string s = "") {
  if (s.empty()) {
    std::cout << "empty string" << std::endl;
  } else {
    std::cout << s << std::endl;
  }
}

int main() {
  foo("hello");
  foo(NULL);
  bar("ciao");
  bar("");
  bar();
}

