#include "timed_event.h"
#include "gtest/gtest.h"

TEST(TimedEventTest, StaticAllocation) {
  std::string value = std::string("hello");
  mu::TimedEvent *te = mu::TimedEvent::create(1.0, &value);

  EXPECT_EQ(1.0, te->getTime());
  EXPECT_STREQ("hello", ((std::string *)te->getValue())->c_str());
}

TEST(TimedEventTest, Comparison) {
  mu::TimedEvent *t0 = mu::TimedEvent::create(0.0, NULL);
  mu::TimedEvent *t1 = mu::TimedEvent::create(1.0, NULL);
  mu::TimedEvent *t2 = mu::TimedEvent::create(1.0, NULL);
  mu::TimedEvent *t3 = mu::TimedEvent::create(2.0, NULL);

  EXPECT_TRUE(t0->operator<(t1));
  EXPECT_TRUE(t0->operator<(t3));
  EXPECT_TRUE(t1->operator<(t3));
  EXPECT_FALSE(t1->operator<(t0));
  EXPECT_FALSE(t1->operator<(t2));
}
