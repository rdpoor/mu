/*
 * PsiStream needs triplets:
 *   sample
 *   delta sample
 *   period
 * which requires heavy pre-computation.  Once we have that
 * data, how quickly can we read it?
 *
 * Answer: a million lines in under 1.5 seconds.
 */
#include <cstdlib>
#include <stdio.h>
#include <sys/time.h>

double frand() {
  return ((double) rand()) / RAND_MAX;
}

void create_test_file(const char* filename, int n_lines) {
  FILE* ofd = fopen(filename, "w");
  if (NULL == ofd) {
    fprintf(stderr, "Failed to open %s for writing\n", filename);
    return;
  }
  fprintf(ofd, "# this is a comment line\n");
  for (int i=0; i<n_lines; i++) {
    fprintf(ofd, "%la, %la, %la\n", frand(), frand(), frand());
  }
  fprintf(stderr, "Wrote %d items to %s.\n", n_lines, filename);
  fclose(ofd);
}

void parse_test_file(const char* filename) {
  double x, y, z;
  int n_items_read, n_lines_read;

  FILE* ifd = fopen(filename, "r");
  if (NULL == ifd) {
    fprintf(stderr, "Failed to open %s for reading.\n", filename);
    return;
  }
  
  n_lines_read = 0;
  char *line = NULL;
  size_t linecap = 0;
  while (getline(&line, &linecap, ifd) != EOF) {
    n_items_read = sscanf(line, "%la, %la, %la\n", &x, &y, &z);
    if (n_items_read != 3) {
      fprintf(stderr, "Skipping line %d, n_items_read = %d\n", n_lines_read, n_items_read);
    }
    n_lines_read += 1;
  }
  fprintf(stderr, "Read %d lines from %s.\n", n_lines_read, filename);
  fclose(ifd);
}

#define FILENAME "/tmp/triplets.txt"
  
int main( void ) {
  struct timeval timeStart, timeEnd;

  create_test_file(FILENAME, 1000000);

  gettimeofday(&timeStart, NULL);  
  parse_test_file(FILENAME);
  gettimeofday(&timeEnd, NULL);

  fprintf(stderr, "parsed in %ld uSec\n", 
          ((timeEnd.tv_sec - timeStart.tv_sec) * 1000000 + timeEnd.tv_usec - timeStart.tv_usec));
  return 0;
}

