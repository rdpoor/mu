#ifndef MU_ASSERT_H
#define MU_ASSERT_H

void assert(bool test, const char *msg1);

#define ASSERT(_X_) assert(_X_, #_X_)

#endif
