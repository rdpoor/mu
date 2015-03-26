#include <functional>
#include <iostream>

// using function pointer
template <typename T>
T test1(T arg, T (*mapfn)(T)) {
  return mapfn(arg);
}

// using a lambda
template <typename T>
T test2(T arg, std::function<T (T)> mapfn) {
  return mapfn(arg);
}

// using a lambda with explicit return type
template <typename T, typename MapFn>
T test3(T arg, MapFn mapfn) {
  return mapfn(arg);
}

int dbl(int v) {
  return 2 * v;
}

int main() {
  // v1a: compiles, runs without error
  int v1a = test1<int>(11, dbl);
  std::cout << v1a << std::endl;

  // v2a: compiles, runs without error
  int v2a = test2<int>(11, [=](int arg) { return 2 * arg; });
  std::cout << v2a << std::endl;

  int v3a = test3<int>(11, [=](int arg) { return 2 * arg; });
  std::cout << v3a << std::endl;

  // v1b (without template type): compiles, runs without error
  int v1b = test1(11, dbl);
  std::cout << v1b << std::endl;

  // v2a (without template type): doesn't compile: 
  // int v2b = test2(11, [=](int arg)->int { return 2 * arg; });
  // std::cout << v2b << std::endl;

  int v3b = test3(11, [=](int arg)->int { return 2 * arg; });
  std::cout << v3b << std::endl;

  int v3c = test3(11, dbl);
  std::cout << v3c << std::endl;

}
