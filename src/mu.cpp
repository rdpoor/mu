#include "mu.h"

namespace mu {

  double dbToValue(double db) {
    return pow(10, db / 20.0);
  }

}


