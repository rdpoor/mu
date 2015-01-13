/*
 * Making a fluent interface with a base class
 * see http://stackoverflow.com/questions/2768282/how-to-implement-fluent-interface-with-a-base-class-in-c
 */
#include <iostream>

template <typename T> class Base {
public:
  Base() : x_ ( 0 ) { }
  T& setX(int x) { x_ = x; return *static_cast<T *>(this); }
protected:
  int x_;
};
  
#if 0
// sketch_fluent.cpp:17:42: error: no viable conversion from 'A11 *' to 'A11'
//   A11 setY(int a11) { a11_ = a11; return this; }
class A11 : public Base<A11> {
public:
  A11 setY(int a11) { a11_ = a11; return this; }
protected:
  int a11_;
};
#endif

#if 0
// sketch_fluent.cpp:24:43: error: non-const lvalue reference to type 'A12' cannot bind to a temporary of type 'A12 *'
//   A12 &setY(int a12) { a12_ = a12; return this; }
class A12 : public Base<A12> {
public:
  A12 &setY(int a12) { a12_ = a12; return this; }
protected:
  int a12_;
};
#endif

class A13 : public Base<A13> {
public:
  A13 *setY(int a13) { a13_ = a13; return this; }
protected:
  int a13_;
};

#if 0
// sketch_fluent.cpp:38:42: error: cannot take the address of an rvalue of type 'B11 *'
//   B11 setY(int b11) { b11_ = b11; return &this; }
class B11 : public Base<B11> {
public:
  B11 setY(int b11) { b11_ = b11; return &this; }
protected:
  int b11_;
};
#endif

#if 0
// sketch_fluent.cpp:45:43: error: cannot take the address of an rvalue of type 'B12 *'
//   B12 &setY(int b12) { b12_ = b12; return &this; }
//                                           ^~~~~
class B12 : public Base<B12> {
public:
  B12 &setY(int b12) { b12_ = b12; return &this; }
protected:
  int b12_;
};
#endif

#if 0
// sketch_fluent.cpp:52:43: error: cannot take the address of an rvalue of type 'B13 *'
//   B13 *setY(int b13) { b13_ = b13; return &this; }
//                                           ^~~~~
class B13 : public Base<B13> {
public:
  B13 *setY(int b13) { b13_ = b13; return &this; }
protected:
  int b13_;
};
#endif

class C11 : public Base<C11> {
public:
  C11 setY(int c11) { c11_ = c11; return *this; }
protected:
  int c11_;
};

class C12 : public Base<C12> {
public:
  C12 &setY(int c12) { c12_ = c12; return *this; }
protected:
  int c12_;
};

#if 0
// sketch_fluent.cpp:73:43: error: no viable conversion from 'C13' to 'C13 *'
//   C13 *setY(int c13) { c13_ = c13; return *this; }
//                                           ^~~~~
class C13 : public Base<C13> {
public:
  C13 *setY(int c13) { c13_ = c13; return *this; }
protected:
  int c13_;
};
#endif

int main(void) {
  // A11 a11;
  // A12 a12;
  A13 a13;
  // A13 &a13r = new A13();
  A13 *a13p = new A13();

  // B11 b11;
  // B12 b12;
  // B13 b13;

  C11 c11;
  // C11 &c11r = new C11();
  C11 *c11p = new C11();

  C12 c12;
  // C12 &c12r = new C12();
  C12 *c12p = new C12();
  // C13 c13;


  return 0;
}
