#include <vector>
#include "mu.h"
#include "identity_stream.h"
#include "file_read_stream.h"

typedef std::vector<mu::Stream *> stream_vector;

int main() {
  mu::IdentityStream s1;
  mu::FileReadStream s2;
  stream_vector v1;

  fprintf(stderr, "LONG_MAX = %ld, LONG_MIN = %ld\n", LONG_MAX, LONG_MIN);

  fprintf(stderr, "s1 = %p, s2 = %p\n", &s1, &s2);

  fprintf(stderr, "v1.size=%ld, v1.max_size=%ld, v1.capacity=%ld\n", v1.size(), v1.max_size(), v1.capacity());

  v1.push_back(&s1);
  v1.push_back(&s2);

  fprintf(stderr, "v1.size=%ld, v1.max_size=%ld, v1.capacity=%ld\n", v1.size(), v1.max_size(), v1.capacity());

  for (long int i=0; i<v1.size(); i++) {
    printf("v[%ld] = %p\n", i, v1.at(i));
  }

  v1.pop_back();

  fprintf(stderr, "v1.size=%ld, v1.max_size=%ld, v1.capacity=%ld\n", v1.size(), v1.max_size(), v1.capacity());

  for (long int i=0; i<v1.size(); i++) {
    printf("v[%ld] = %p\n", i, v1.at(i));
  }

  return 0;
}
