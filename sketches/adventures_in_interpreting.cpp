/*
 * The Mu server should be able to eval() a string (or a JSON
 * structure) in order to create, manipulate and destroy objects.
 */
// #include <cstdlib>
#include <stdio.h>

class MyClass {
public:
  int value() {
    return value_;
  }
  void set_value(int value) {
    value_ = value;
  }
protected:
  int value_;
};

// without an intepreter
void test01() {
  MyClass *a1 = new MyClass();
  MyClass *a2 = new MyClass();
  a1->set_value(22);
  a2->set_value(23);
}

// Can I get the pointer to one of A's member functions and then call it?
// Yes: http://stackoverflow.com/a/8865807/558639
void test02() {
  typedef void (MyClass::*AFunctionPointer)(int x);
  AFunctionPointer fnc_pointer = &MyClass::set_value;

  MyClass a1;
  (a1.*fnc_pointer)(24);
  printf("a1.value() = %d\n", a1.value());
}

// um, without the typedef
// see also http://www.parashift.com/c++-faq-lite/fnptr-vs-memfnptr-types.html
void test03() {
  void (MyClass::*fnc_pointer)(int) = &MyClass::set_value;

  MyClass a1;
  (a1.*fnc_pointer)(25);
  printf("a1.value() = %d\n", a1.value());
}

// Thoughts:
// The intepreter can only interact with objects and their methods: 
// given an object name and member function name, call invoke that
// member function with that object.
// 
// Objects are named.  Given a name, e.g. a1 in the example above, we
// need to look up a reference to the object itself (&a1) and a
// reference to the class.  Given the class and the member function
// name, we need to look up a function pointer.

// register_object_reference()
// register_class_method()
// invoke_function(object_name, method_name, args)

#include <unordered_map>
#include <string>
using namespace std;

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

typedef unordered_map<string, void *> ClassMethodMap;

class MuValuator {
public:
  void register_object(void *object, std::string object_name, std::string class_name);
  void register_method(void *fn_ptr, std::string method_name, std::string class_name);
  void invoke_method(std::string object_name, std::string method_name, void **args);

protected:
  ClassMethodMap *find_or_create_method_map(string class_name);

  unordered_map<string, void *> object_reference_map_;       // map object name to object reference
  unordered_map<string, string> object_class_map_;           // map object name to a class name
  unordered_map<string, ClassMethodMap *> class_method_map_; // map class name to method => function_pointer map
};

void MuValuator::register_object(void *object, std::string object_name, std::string class_name) {
  object_reference_map_.insert(make_pair(object_name, object));
  object_class_map_.insert(make_pair(object_name, class_name));
}

void MuValuator::register_method(void *fn_ptr, std::string method_name, std::string class_name) {
  ClassMethodMap *method_map = MuValuator::find_or_create_method_map(class_name);
  method_map->insert(make_pair(method_name, fn_ptr));
}

ClassMethodMap *MuValuator::find_or_create_method_map(string class_name) {
  unordered_map<string, ClassMethodMap *>::const_iterator got = class_method_map_.find(class_name);
  if (got == class_method_map_.end()) {
    ClassMethodMap *method_map = new ClassMethodMap;
    class_method_map_.insert(make_pair(class_name, method_map));
    return method_map;
  } else {
    return got->second;
  }
}

void MuValuator::invoke_method(std::string object_name, std::string method_name, void **args) {
  void *object_ref = object_reference_map_.find(object_name)->second;
  string class_name = object_class_map_.find(object_name)->second;
  unordered_map<string, void *> *method_map = class_method_map_.find(class_name)->second;
  void *method_ref = method_map->find(method_name)->second;

  printf("object_name = %s, object_ref = %p, class_name = %s, method_name = %s, method_ref = %p\n", 
         object_name.c_str(), object_ref, class_name.c_str(), method_name.c_str(), method_ref);
}


int main() {
  MuValuator e;

  MyClass a1;
  e.register_object(&a1, "a1", "MyClass");
  e.register_method(&MyClass::value, "value", "MyClass");
  e.register_method(&MyClass::set_value, "set_value", "MyClass");
  e.invoke_method("a1", "set_value", NULL);
  test02();
  test03();
  return 0;
}
