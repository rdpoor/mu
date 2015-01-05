#include "timed_queue.h"
#include "gtest/gtest.h"

class TimedQueueTest : public ::testing::Test {
protected:
  virtual void SetUp() {
    e0_ = mu::TimedEvent::create(0.5, NULL);
    e1_ = mu::TimedEvent::create(1.0, NULL);
    e2_ = mu::TimedEvent::create(2.0, NULL);
    q0_ = new mu::TimedQueue();
    q1_ = new mu::TimedQueue();
  }

  mu::TimedQueue *q0_;
  mu::TimedQueue *q1_;
  mu::TimedEvent *e0_;
  mu::TimedEvent *e1_;
  mu::TimedEvent *e2_;
};

TEST_F(TimedQueueTest, InitiallyEmpty) {
  EXPECT_EQ(0, q0_->size());
  EXPECT_EQ(NULL, q0_->peekNext());
  EXPECT_EQ(NULL, q0_->getNext());

  q0_->enqueue(e0_);
  EXPECT_EQ(1, q0_->size());
}

TEST_F(TimedQueueTest, EnqueueReturnsThis) {
  EXPECT_EQ(q0_, q0_->enqueue(e0_));
}

TEST_F(TimedQueueTest, InitiallyNonEmpty) {
  mu::TimedEvent *e;

  q1_->enqueue(e1_);
  EXPECT_EQ(1, q1_->size());

  e = q1_->peekNext();
  EXPECT_EQ(e1_->getTime(), e->getTime());
  EXPECT_EQ(1, q1_->size());

  e = q1_->getNext();
  EXPECT_EQ(e1_->getTime(), e->getTime());
  EXPECT_EQ(0, q1_->size());
}

TEST_F(TimedQueueTest, ClearQueue) {
  q1_->enqueue(e1_);
  EXPECT_EQ(1, q1_->size());
  q1_->clear();
  EXPECT_EQ(0, q1_->size());
}

TEST_F(TimedQueueTest, MaintainsOrder) {
  q1_->enqueue(e1_);
  q1_->enqueue(e2_);
  q1_->enqueue(e0_);

  EXPECT_EQ(e0_->getTime(), q1_->getNext()->getTime());
  EXPECT_EQ(e1_->getTime(), q1_->getNext()->getTime());
  EXPECT_EQ(e2_->getTime(), q1_->getNext()->getTime());
  EXPECT_EQ(0, q1_->size());
}

TEST_F(TimedQueueTest, StrictOrdering) {
  mu::TimedEvent *e3 = mu::TimedEvent::create(1.0, NULL);
  q1_->enqueue(e2_);
  q1_->enqueue(e1_);
  q1_->enqueue(e0_);
  q1_->enqueue(e3);

  EXPECT_EQ(e0_, q1_->getNext());
  EXPECT_EQ(e1_, q1_->getNext());
  EXPECT_EQ(e3, q1_->getNext());
  EXPECT_EQ(e2_, q1_->getNext());

  q1_->enqueue(e3);
  q1_->enqueue(e0_);
  q1_->enqueue(e1_);
  q1_->enqueue(e2_);

  EXPECT_EQ(e0_, q1_->getNext());
  EXPECT_EQ(e3, q1_->getNext());
  EXPECT_EQ(e1_, q1_->getNext());
  EXPECT_EQ(e2_, q1_->getNext());
}
