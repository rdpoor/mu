/*
 * Test out allocation pool.  
 */
#include <unistd.h>             // sleep()
#include <sched.h>              // sched_yield()
#include <pthread.h>            // pthread
#include <stdio.h>              // printf
#include "mu_pool.h"

// ================================================================
// sample

class Widget {
public:
  Widget() : value_(-1) {}
  int value() { return value_;  }
protected:
  int value_;
};

bool g_stop;
mu::MuPool<Widget> pool_x;
mu::MuPool<Widget> pool_y;

// In a thread, repeatedly allocate widgets from the source pool and
// save them in the sink pool.  Yield to other thread(s).
void *runner(mu::MuPool<Widget> *source, mu::MuPool<Widget> *sink) {
  while (g_stop == false) {
    sink->deallocate(source->allocate());
    sched_yield();
  }
  return NULL;
}

// Allocate from pool_x and save in pool_y
void *runner1(void *unused) {
  return runner(&pool_x, &pool_y);
}

// Allocate from pool_y and save in pool_x
void *runner2(void *unused) {
  return runner(&pool_y, &pool_x);
}

// ================================================================
//
#define SLEEP_TIME 10

int main() {

  pthread_t thread1, thread2;
  int rc;


  g_stop = false;
  rc = pthread_create(&thread1, NULL, runner1, NULL);
  if (rc) {
    printf("Error: return code from pthread_create() = %d\n", rc);
  }
  rc = pthread_create(&thread2, NULL, runner2, NULL);
  if (rc) {
    printf("Error: return code from pthread_create() = %d\n", rc);
  }
  sleep(SLEEP_TIME);
  g_stop = true;
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  printf("pool_x: size(), created(), called() = %d, %d, %d\n", pool_x.size(), pool_x.created(), pool_x.called());
  printf("pool_y: size(), created(), called() = %d, %d, %d\n", pool_y.size(), pool_y.created(), pool_y.called());
  printf("items leaked = %d, alloc/sec = %f\n",
         (pool_x.created() + pool_y.created()) - (pool_x.size() + pool_y.size()),
         1.0 * (pool_x.called() + pool_y.called()) / SLEEP_TIME);
    
}
