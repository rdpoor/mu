/*
 * Unit Test StreamExtent
 */
#include "assert.h"
#include "mu.h"
#include "stream.h"
#include <unistd.h>

// ================
class S1 : public mu::Stream {
public:
  S1& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) { return *this; }
protected:
};

// ================
class S2 : public mu::Stream {
public:
  S2& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) { return *this; }
  mu::Tick getStart() { return 42; }
  mu::Tick getEnd() { return 44; }
protected:
};

// ================
class S3 : public mu::Stream {
public:
  S3& step(stk::StkFrames& buffer, mu::Tick tick, mu::Player &player) { return *this; }
  mu::Tick getStart( void ) { return start_; }
  S3& setStart( mu::Tick start) { start_ = start; return *this; }
  mu::Tick getEnd( void ) { return end_; }
  S3& setEnd( mu::Tick end) { end_ = end; return *this; }
protected:
  mu::Tick start_;
  mu::Tick end_;
};

int main() {
  S1 s1;
  S2 s2;
  S3 s3;

  ASSERT(s1.getStart() == mu::kIndefinite);
  ASSERT(s1.getEnd() == mu::kIndefinite);
  ASSERT(s1.getDuration() == mu::kIndefinite);

  ASSERT(s2.getStart() == 42);
  ASSERT(s2.getEnd() == 44);
  ASSERT(s2.getDuration() == 2);

  s3.setStart(52).setEnd(mu::kIndefinite);
  ASSERT(s3.getStart() == 52);
  ASSERT(s3.getEnd() == mu::kIndefinite);
  ASSERT(s3.getDuration() == mu::kIndefinite);

  s3.setStart(52).setEnd(56);
  ASSERT(s3.getStart() == 52);
  ASSERT(s3.getEnd() == 56);
  ASSERT(s3.getDuration() == 4);

  return 0;
}
