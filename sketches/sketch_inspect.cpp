#include <stdio.h>
#include <unistd.h>
#include "mu.h"
#include "stream.h"
#include "add_stream.h"
#include "identity_stream.h"

int main() {
  mu::AddStream add_stream;
  mu::IdentityStream is1;
  mu::IdentityStream is2;

  add_stream.addSource(&is1);
  add_stream.addSource(&is2);
  std::cout << add_stream.inspect();
  
  return 0;
}

  
