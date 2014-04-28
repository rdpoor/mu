/*
 * Test SampleBuffer
 */
#include "mu.h"
#include "sample_buffer.h"
#include <stdio.h>

void assert(bool b, const char *msg) {
  printf("%s: %s\n", msg, (b?"true":"false"));
}

int main() {
  stk::StkFrames sf1;            // frames
  mu::SampleBuffer sb1;          // 

  // single channel
  sf1.resize(100, 1);
  sf1[10] = 2.2;
  sf1[15] = 4.4;
  assert(sf1[10] == 2.2, "sf1[10] == 2.2");
  assert(sf1[15] == 4.4, "sf1[15] == 4.4");

  sb1.slice(sf1);
  assert(sb1[10] == sf1[10], "sb1[10] == sf1[10]");
  assert(sb1[15] == sf1[15], "sb1[15] == sf1[15]");

  sb1.slice(sf1,10);
  assert(sb1[0] == sf1[10], "sb1[0] == sf1[10]");
  assert(sb1[5] == sf1[15], "sb1[5] == sf1[15]");

  assert(sf1.frames() == 100, "sf1.frames() == 100");
  assert(sf1.size() == 100, "sf1.size() == 100");
  assert(sb1.frames() == 90, "sb1.frames() == 90");
  assert(sb1.size() == 90, "sb1.size() == 90");

  // two channel
  sf1.resize(100, 2);
  sf1(10, 0) = 2.5;
  sf1(10, 1) = 2.5;
  sf1(15, 0) = 4.5;
  sf1(15, 1) = 4.6;
  assert(sf1(10, 0) == 2.5, "sf1(10, 0) == 2.5");
  assert(sf1(10, 1) == 2.5, "sf1(10, 1) == 2.5");
  assert(sf1(15, 0) == 4.5, "sf1(15, 0) == 4.5");
  assert(sf1(15, 1) == 4.6, "sf1(15, 1) == 4.6");

  sb1.slice(sf1);
  assert(sb1(10, 0) == sf1(10, 0), "sb1(10, 0) == sf1(10, 0)");
  assert(sb1(10, 1) == sf1(10, 1), "sb1(10, 1) == sf1(10, 1)");
  assert(sb1(15, 0) == sf1(15, 0), "sb1(15, 0) == sf1(15, 0)");
  assert(sb1(15, 1) == sf1(15, 1), "sb1(15, 1) == sf1(15, 1)");

  sb1.slice(sf1, 10);
  assert(sb1(0, 0) == sf1(10, 0), "sb1(0, 0) == sf1(10, 0)");
  assert(sb1(0, 1) == sf1(10, 1), "sb1(0, 1) == sf1(10, 1)");
  assert(sb1(5, 0) == sf1(15, 0), "sb1(5, 0) == sf1(15, 0)");
  assert(sb1(5, 1) == sf1(15, 1), "sb1(5, 1) == sf1(15, 1)");

  assert(sf1.frames() == 100, "sf1.frames() == 100");
  assert(sf1.size() == 200, "sf1.size() == 200");
  assert(sb1.frames() == 90, "sb1.frames() == 90");
  assert(sb1.size() == 180, "sb1.size() == 180");

  return 0;
}
