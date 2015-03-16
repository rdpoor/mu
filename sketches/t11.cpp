// A pencil sketch for a simple interpreter.  Currently only handles
// calls to an object's member function with no args and void return
// type.

#include <stdio.h>
#include <unordered_map>
#include <string>
using namespace std;

typedef unordered_map<string, void (*)()> MethodNameToFunctionPointerMap;

class MuValuator {
public:
  // Associate an object name and class name with an object reference
  void register_object(void *object, std::string object_name, std::string class_name);

  // Associate a class name / method name with a function pointer.
  void register_method(void (*fn_ptr)(), std::string method_name, std::string class_name);

  // Given a previously registered object name, invoke its member function.
  void invoke_method(std::string object_name, std::string method_name);

protected:
  // Find (or create) a map that associates method names with function pointers
  // for the given class name.
  MethodNameToFunctionPointerMap *find_or_create_method_map(string class_name);

  unordered_map<string, void *> object_reference_map_;       // map object name to object reference
  unordered_map<string, string> object_class_map_;           // map object name to a class name
  unordered_map<string, MethodNameToFunctionPointerMap *> class_method_map_; // map class name to method => function_pointer map
};

void MuValuator::register_object(void *object, std::string object_name, std::string class_name) {
  object_reference_map_.insert(make_pair(object_name, object));
  object_class_map_.insert(make_pair(object_name, class_name));
}

void MuValuator::register_method(void (*fn_ptr)(), std::string method_name, std::string class_name) {
  MethodNameToFunctionPointerMap *method_map = MuValuator::find_or_create_method_map(class_name);
  method_map->insert(make_pair(method_name, fn_ptr));
}

MethodNameToFunctionPointerMap *MuValuator::find_or_create_method_map(string class_name) {
  unordered_map<string, MethodNameToFunctionPointerMap *>::const_iterator got = class_method_map_.find(class_name);
  if (got == class_method_map_.end()) {
    // not found: create the method map for class_name
    MethodNameToFunctionPointerMap *method_map = new MethodNameToFunctionPointerMap;
    class_method_map_.insert(make_pair(class_name, method_map));
    return method_map;
  } else {
    // found: return the method map for class_name
    return got->second;
  }
}

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

void MuValuator::invoke_method(std::string object_name, std::string method_name) {
  void *object_ref = object_reference_map_.find(object_name)->second;
  string class_name = object_class_map_.find(object_name)->second;
  MethodNameToFunctionPointerMap *method_map = class_method_map_.find(class_name)->second;
  void (*method_ref)() = method_map->find(method_name)->second;

  printf("object_name = %s, object_ref = %p, class_name = %s, method_name = %s, method_ref = %p\n", 
         object_name.c_str(), object_ref, class_name.c_str(), method_name.c_str(), method_ref);

  // Not ready for prime time...
  // CALL_MEMBER_FN(object_ref, method_ref);
}

// ================================================================
// A class to test things on

class MyClass {
public:
  void method_1() {
    printf("%p: MyClass::method_1()\n", this);
  }
  void method_2() {
    printf("%p: MyClass::method_1()\n", this);
  }
protected:
  int value_;
};

// ================================================================
// main()

int main() {
  MuValuator e;

  MyClass a1;
  e.register_object(&a1, "a1", "MyClass");
  // Compile time error on the next line:
  // error: cannot initialize a parameter of type 'void (*)()' with an rvalue of type 'void (MyClass::*)()'
  e.register_method(&MyClass::method_1, "method_1", "MyClass");
  e.register_method(&MyClass::method_2, "method_2", "MyClass");

  e.invoke_method("a1", "method_1");
  return 0;
}
