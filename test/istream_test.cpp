#include "gmock/gmock.h"
#include "istream.h"
#include <vector>

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

typedef mu::IStream<int> IntStream;

IntStream *ints(int initial) {
  return IntStream::Increment(initial);
}

std::vector<int> *istream_to_vector(IntStream *s, int maxcount = 100) {
  return IntStream::to_vector(s, maxcount);
}

TEST(IStreamTest, IntegersIS) {
  ASSERT_THAT(*istream_to_vector(ints(100), 5), 
              ElementsAre(100, 101, 102, 103, 104));
}

// ================================================================
// Take

TEST(IStreamTest, Take0) {
  ASSERT_THAT(*istream_to_vector(IntStream::Take(ints(100), 5), 100),
              ElementsAre(100, 101, 102, 103, 104));
}

TEST(IStreamTest, Take1) {
  ASSERT_THAT(*istream_to_vector(IntStream::Take(NULL_ISTREAM, 5), 100),
              ElementsAre());
}

// ================================================================
// Append

TEST(IStreamTest, Append0) {
  IntStream *s0 = IntStream::Take(ints(100), 4);
  IntStream *s1 = ints(10);
  ASSERT_THAT(*istream_to_vector(IntStream::Append(s0, s1), 8),
              ElementsAre(100, 101, 102, 103, 10, 11, 12, 13));
}

TEST(IStreamTest, Append1) {
  IntStream *s0 = NULL_ISTREAM;
  IntStream *s1 = ints(10);
  ASSERT_THAT(*istream_to_vector(IntStream::Append(s0, s1), 8),
              ElementsAre(10, 11, 12, 13, 14, 15, 16, 17));
}

TEST(IStreamTest, Append2) {
  IntStream *s0 = IntStream::Take(ints(100), 4);
  IntStream *s1 = NULL_ISTREAM;
  ASSERT_THAT(*istream_to_vector(IntStream::Append(s0, s1), 8),
              ElementsAre(100, 101, 102, 103));
}

TEST(IStreamTest, Append3) {
  IntStream *s0 = NULL_ISTREAM;
  IntStream *s1 = NULL_ISTREAM;
  ASSERT_THAT(*istream_to_vector(IntStream::Append(s0, s1), 8),
              ElementsAre());
}

// ================================================================
// Filter

TEST(IStreamTest, Filter0) {
  IntStream *s = IntStream::Filter(ints(100), 
                                   [=](int i) { return !(i % 2); });
  ASSERT_THAT(*istream_to_vector(s, 5),
              ElementsAre(100, 102, 104, 106, 108));
}

TEST(IStreamTest, Filter1) {
  IntStream *s = IntStream::Filter(NULL_ISTREAM, 
                                 [=](int i) { return !(i % 2); });
  ASSERT_THAT(*istream_to_vector(s, 5),
              ElementsAre());
}

// ================================================================
// Map

TEST(IStreamTest, Map0) {
  IntStream *s = IntStream::Map(ints(100), [=](int i){ return 2 * i; });
  ASSERT_THAT(*istream_to_vector(s, 5),
              ElementsAre(200, 202, 204, 206, 208));
}

TEST(IStreamTest, Map1) {
  ASSERT_THAT(*istream_to_vector(NULL_ISTREAM, 5), ElementsAre());
}

// ================================================================
// Merge

TEST(IStreamTest, Merge0) {
  IntStream *s0 = IntStream::Map(ints(0), [=](int i){ return 4*i; });  
  IntStream *s1 = IntStream::Map(ints(0), [=](int i){ return 5*i; });
  IntStream *merged = IntStream::Merge(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*istream_to_vector(merged, 12),
              ElementsAreArray({0, 0, 4, 5, 8, 10, 12, 15, 16, 20, 20, 24}));
}

TEST(IStreamTest, Merge1) {
  IntStream *s0 = IntStream::Map(ints(0), [=](int i){ return 4*i; });  
  IntStream *s1 = NULL_ISTREAM;
  IntStream *merged = IntStream::Merge(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*istream_to_vector(merged, 12),
              ElementsAreArray({0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44}));
}

TEST(IStreamTest, Merge2) {
  IntStream *s0 = NULL_ISTREAM;
  IntStream *s1 = IntStream::Map(ints(0), [=](int i){ return 5*i; });
  IntStream *merged = IntStream::Merge(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*istream_to_vector(merged, 12),
              ElementsAreArray({0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55}));
}

TEST(IStreamTest, Merge5) {
  IntStream *s0 = NULL_ISTREAM;
  IntStream *s1 = NULL_ISTREAM;
  IntStream *merged = IntStream::Merge(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*istream_to_vector(merged, 12), ElementsAre());
}

// ================================================================
// Repeat

TEST(IStreamTest, Repeat0) {
  IntStream *s = IntStream::Repeat(IntStream::Take(ints(10), 4));
  ASSERT_THAT(*istream_to_vector(s, 10),
              ElementsAreArray({10, 11, 12, 13, 10, 11, 12, 13, 10, 11}));
}

TEST(IStreamTest, Repeat1) {
  IntStream *s = IntStream::Repeat(NULL_ISTREAM);
  ASSERT_THAT(*istream_to_vector(s, 12), ElementsAre());
}


// ================================================================
