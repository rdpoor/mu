#include "gmock/gmock.h"
#include "estream.h"
#include <vector>

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

typedef mu::EStream<int> IStream;

IStream *ints(int initial) {
  return IStream::IncrementES(initial);
}

std::vector<int> *estream_to_vector(IStream *s, int maxcount = 100) {
  return IStream::to_vector(s, maxcount);
}

TEST(EStreamTest, ElementComparison) {
  std::vector<int> *v = new std::vector<int>({1, 2, 3});
  ASSERT_THAT(*v, ElementsAreArray({1, 2, 3}));
  ASSERT_THAT(*v, ElementsAre(1, 2, 3));
}

TEST(EStreamTest, IntegersES) {
  ASSERT_THAT(*estream_to_vector(ints(100), 5), 
              ElementsAre(100, 101, 102, 103, 104));
}

TEST(EStreamTest, TakeES0) {
  ASSERT_THAT(*estream_to_vector(IStream::TakeES(ints(100), 5), 100),
              ElementsAre(100, 101, 102, 103, 104));
}

TEST(EStreamTest, TakeES1) {
  ASSERT_THAT(*estream_to_vector(IStream::TakeES(NULL_ESTREAM, 5), 100),
              ElementsAre());
}

// ================================================================
// AppendES

TEST(EStreamTest, AppendES0) {
  IStream *s0 = IStream::TakeES(ints(100), 4);
  IStream *s1 = ints(10);
  ASSERT_THAT(*estream_to_vector(IStream::AppendES(s0, s1), 8),
              ElementsAre(100, 101, 102, 103, 10, 11, 12, 13));
}

TEST(EStreamTest, AppendES1) {
  IStream *s0 = NULL_ESTREAM;
  IStream *s1 = ints(10);
  ASSERT_THAT(*estream_to_vector(IStream::AppendES(s0, s1), 8),
              ElementsAre(10, 11, 12, 13, 14, 15, 16, 17));
}

TEST(EStreamTest, AppendES2) {
  IStream *s0 = IStream::TakeES(ints(100), 4);
  IStream *s1 = NULL_ESTREAM;
  ASSERT_THAT(*estream_to_vector(IStream::AppendES(s0, s1), 8),
              ElementsAre(100, 101, 102, 103));
}

TEST(EStreamTest, AppendES3) {
  IStream *s0 = NULL_ESTREAM;
  IStream *s1 = NULL_ESTREAM;
  ASSERT_THAT(*estream_to_vector(IStream::AppendES(s0, s1), 8),
              ElementsAre());
}

// ================================================================
// FilterES

TEST(EStreamTest, FilterES0) {
  IStream *s = IStream::FilterES(ints(100), 
                                 [=](int i) { return !(i % 2); });
  ASSERT_THAT(*estream_to_vector(s, 5),
              ElementsAre(100, 102, 104, 106, 108));
}

TEST(EStreamTest, FilterES1) {
  IStream *s = IStream::FilterES(NULL_ESTREAM, 
                                 [=](int i) { return !(i % 2); });
  ASSERT_THAT(*estream_to_vector(s, 5),
              ElementsAre());
}

// ================================================================
// MapES

TEST(EStreamTest, MapES0) {
  IStream *s = IStream::MapES(ints(100), [=](int i){ return 2 * i; });
  ASSERT_THAT(*estream_to_vector(s, 5),
              ElementsAre(200, 202, 204, 206, 208));
}

TEST(EStreamTest, MapES1) {
  ASSERT_THAT(*estream_to_vector(NULL_ESTREAM, 5), ElementsAre());
}

// ================================================================
// MergeES

TEST(EStreamTest, MergeES0) {
  IStream *s0 = IStream::MapES(ints(0), [=](int i){ return 4*i; });  
  IStream *s1 = IStream::MapES(ints(0), [=](int i){ return 5*i; });
  IStream *merged = IStream::MergeES(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*estream_to_vector(merged, 12),
              ElementsAreArray({0, 0, 4, 5, 8, 10, 12, 15, 16, 20, 20, 24}));
}

TEST(EStreamTest, MergeES1) {
  IStream *s0 = IStream::MapES(ints(0), [=](int i){ return 4*i; });  
  IStream *s1 = NULL_ESTREAM;
  IStream *merged = IStream::MergeES(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*estream_to_vector(merged, 12),
              ElementsAreArray({0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44}));
}

TEST(EStreamTest, MergeES2) {
  IStream *s0 = NULL_ESTREAM;
  IStream *s1 = IStream::MapES(ints(0), [=](int i){ return 5*i; });
  IStream *merged = IStream::MergeES(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*estream_to_vector(merged, 12),
              ElementsAreArray({0, 5, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55}));
}

TEST(EStreamTest, MergeES5) {
  IStream *s0 = NULL_ESTREAM;
  IStream *s1 = NULL_ESTREAM;
  IStream *merged = IStream::MergeES(s0, 
                                     s1, 
                                     [=](int v0, int v1) {return v0 < v1;});
  ASSERT_THAT(*estream_to_vector(merged, 12), ElementsAre());
}

// ================================================================
// RepeatES

TEST(EStreamTest, RepeatES0) {
  IStream *s = IStream::RepeatES(IStream::TakeES(ints(10), 4));
  ASSERT_THAT(*estream_to_vector(s, 10),
              ElementsAreArray({10, 11, 12, 13, 10, 11, 12, 13, 10, 11}));
}

TEST(EStreamTest, RepeatES1) {
  IStream *s = IStream::RepeatES(NULL_ESTREAM);
  ASSERT_THAT(*estream_to_vector(s, 12), ElementsAre());
}


// ================================================================
