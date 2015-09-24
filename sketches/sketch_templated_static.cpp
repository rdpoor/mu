template <typename T>
class A {
public:
  static A *foo() {
    // imagine complex code here
    return new A<T>();
  }
};

// B is a typed specialization of A
class B : public A<int> {
public:
  static B *foo() {
    // return A<int>::foo();    // doesn't compile
    return (B *)A<int>::foo();    // doesn't compile
  }
};

int main() {
  B *b = B::foo();
  (void)b;                      // suppress unused variable warning
}
