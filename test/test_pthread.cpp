#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// not clear when I use pthread_t objects and when I use pointers
// to pthread_t objects...
pthread_t g_current_pthread;

void *process(void *arg) {
  fprintf(stderr,"IN PROCESSING THREAD\n");
  g_current_pthread = pthread_self();
  while (pthread_equal(g_current_pthread, pthread_self())) {
    fprintf(stderr,"BING...");
    sleep(1);
  }
  fprintf(stderr,"LEAVING PROCESSING THREAD...\n");
  return NULL;
}

int main() {
  pthread_t pthread;

  fprintf(stderr,"creating thread...\n");
  int rc = pthread_create(&pthread, NULL, process, (void *)NULL);
  if (rc) {
    fprintf(stderr,"ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
  }
  fprintf(stderr,"sleeping...\n");
  sleep(10);
  fprintf(stderr,"signalling thread...\n");
  g_current_pthread = pthread_self();
  sleep(3);
  fprintf(stderr,"exiting...\n");

  return 0;
}
