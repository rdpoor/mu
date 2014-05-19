#ifndef MU_ASSERT_H
#define MU_ASSERT_H

bool near(double f1, double f2);

void assert(bool test, const char *msg1, const char *file, const int line);

#define ASSERT(_X_) assert(_X_, #_X_, __FILE__, __LINE__)

#endif
