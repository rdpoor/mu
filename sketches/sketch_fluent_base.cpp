#include "sketch_fluent_base.h"
Base::Base() : x_ ( 0 ) {
}

T& Base::setX(int x) { 
  x_ = x; 
  return *static_cast<T *>(this); 
}
