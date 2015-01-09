#include "jansson.h"
#include <stdio.h>
#include <unordered_map>
#include <string>
using namespace std;

// ================================================================
class EvalBase {
public:
  void eval(string form) {
    json_t *root = load_json(form.c_str());
    if (root) {
      eval_json(root);
      json_decref(root);
    }
  }
protected:
  virtual void eval_json(json_t *form) = 0;
  json_t *load_json(const char *text);
};

json_t *EvalBase::load_json(const char *text) {
  json_t *root;
  json_error_t error;
  
  root = json_loads(text, 0, &error);
  
  if (root) {
    return root;
  } else {
    fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
    return (json_t *)0;
  }
}

// ================================================================
class A : public EvalBase {
public:
  void move(double d) {
    printf("A<%p> move %f\n", this, d);
    x_ += cos(theta_) * d;
    y_ += sin(theta_) * d;
  }
  void turn(double d) {
    printf("A<%p> turn %f\n", this, d);
    theta_ += 2 * M_PI * d / 360.0;
  }
  void eval_json(json_t *form) {
    // TODO: even a tiny bit of error checking?
    const char *method = json_string_value(json_array_get(form, 0));
    double arg = json_real_value(json_array_get(form, 1));
    if (strcmp(method, "move") == 0) {
      move(arg);
    } else if (strcmp(method, "turn") == 0) {
      turn(arg);
    } else {
      fprintf(stderr, "unknown method %s\n", method);
    }
  }
protected:
  double x_;
  double y_;
  double theta_;
};

// ================================================================
class B : public EvalBase {
public:
  void move(double d) {
    printf("B<%p> move %f\n", this, d);
    p_ += d;
  }
  void eval_json(json_t *form) {
    // TODO: even a tiny bit of error checking?
    const char *method = json_string_value(json_array_get(form, 0));
    double arg = json_real_value(json_array_get(form, 1));
    if (strcmp(method, "move") == 0) {
      move(arg);
    } else {
      fprintf(stderr, "unknown method %s\n", method);
    }
  }
protected:
  double p_;
};

// ================================================================
int main() {
  A a1, a2;
  B b1;

  a1.eval("[\"move\", 12.0]");
  a2.eval("[\"turn\", 3.14159]");
  b1.eval("[\"move\", 20.0]");

  return 0;
}
