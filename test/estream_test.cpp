#include "gmock/gmock.h"
#include "estream.h"
#include <vector>


using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

std::vector<double> *estream_times(mu::EStream *s, int maxcount = 100) {
  std::vector<double> *v = new std::vector<double>();
  mu::EStream *s0 = s;
  
  while (!IS_NULL_ISTREAM(s0) && (maxcount > 0)) {
    v->push_back(s0->first()->time());
    mu::EStream *next = s0->rest();
    if (s0 != s) delete s0;
    s0 = next;
    maxcount -= 1;
  }
  return v;
}

// ================================================================
// Metronome

TEST(EStreamTest, Metronome0) {
  mu::EStream *s = mu::EStream::Metronome(0.25);
  ASSERT_THAT(*estream_times(s, 8),
              ElementsAreArray({0.0, 0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 1.75}));
}

// ================================================================
// Merge

TEST(EStreamTest, Merge0) {
  mu::EStream *s = mu::EStream::Merge(mu::EStream::Metronome(1.0), 
                                      mu::EStream::Metronome(1.5));
  ASSERT_THAT(*estream_times(s, 10),
              ElementsAreArray({0.0, 0.0, 
                    1.0, 1.5, 
                    2.0, 3.0, 
                    3.0, 4.0, 
                    4.5, 5.0}));
}

                    
