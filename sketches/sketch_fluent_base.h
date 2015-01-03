#ifndef SKETCH_FLUENT_BASE_H
#define SKETCH_FLUENT_BASE_H

template <typename T> class Base {
public:
  Base();
  T& setX(int x);
protected:
  int x_;
};

#endif
