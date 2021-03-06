/*
 * Unit Test SampleProcessor base class
 */
#include "assert.h"
#include "mu.h"
#include "sample_processor.h"
#include <unistd.h>

// ================
class S1 : public mu::SampleProcessor {
public:
  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) { return; }
protected:
};

// ================
class S2 : public mu::SampleProcessor {
public:
  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) { return; }
  mu::Tick getStart() { return 42; }
  mu::Tick getEnd() { return 44; }
protected:
};

// ================
class S3 : public mu::SampleProcessor {
public:
  void step(stk::StkFrames& buffer, mu::Tick tick, bool is_new_event) { return; }
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

  std::cout << s1.inspect();
  // ASSERT(s1.inspect() == "No inspector");

  ASSERT(s1.getStart() == mu::TickUtils::indefinite());
  ASSERT(s1.getEnd() == mu::TickUtils::indefinite());
  ASSERT(s1.getDuration() == mu::TickUtils::indefinite());
  ASSERT(s1.doesContain(0) == true);
  ASSERT(s1.doesContain(42) == true);
  ASSERT(s1.doesContain(44) == true);

  ASSERT(s2.getStart() == 42);
  ASSERT(s2.getEnd() == 44);
  ASSERT(s2.getDuration() == 2);
  ASSERT(s2.doesContain(0) == false);
  ASSERT(s2.doesContain(42) == true);
  ASSERT(s2.doesContain(44) == false);

  s3.setStart(52).setEnd(56);
  ASSERT(s3.getStart() == 52);
  ASSERT(s3.getEnd() == 56);
  ASSERT(s3.getDuration() == 4);
  ASSERT(s3.doesContain(51) == false);
  ASSERT(s3.doesContain(52) == true);
  ASSERT(s3.doesContain(55) == true);
  ASSERT(s3.doesContain(56) == false);
  
  s3.setStart(52).setEnd(mu::TickUtils::indefinite());
  ASSERT(s3.getStart() == 52);
  ASSERT(s3.getEnd() == mu::TickUtils::indefinite());
  ASSERT(s3.getDuration() == mu::TickUtils::indefinite());
  ASSERT(s3.doesContain(51) == false);
  ASSERT(s3.doesContain(52) == true);
  ASSERT(s3.doesContain(55) == true);
  ASSERT(s3.doesContain(56) == true);

  s3.setStart(mu::TickUtils::indefinite()).setEnd(56);
  ASSERT(s3.getStart() == mu::TickUtils::indefinite());
  ASSERT(s3.getEnd() == 56);
  ASSERT(s3.getDuration() == mu::TickUtils::indefinite());
  ASSERT(s3.doesContain(51) == true);
  ASSERT(s3.doesContain(52) == true);
  ASSERT(s3.doesContain(55) == true);
  ASSERT(s3.doesContain(56) == false);
  
  s3.setStart(mu::TickUtils::indefinite()).setEnd(mu::TickUtils::indefinite());
  ASSERT(s3.getStart() == mu::TickUtils::indefinite());
  ASSERT(s3.getEnd() == mu::TickUtils::indefinite());
  ASSERT(s3.getDuration() == mu::TickUtils::indefinite());
  ASSERT(s3.doesContain(51) == true);
  ASSERT(s3.doesContain(52) == true);
  ASSERT(s3.doesContain(55) == true);
  ASSERT(s3.doesContain(56) == true);

  return 0;
}
