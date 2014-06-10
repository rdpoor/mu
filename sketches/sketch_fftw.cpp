/*
 * "hello world" test for FFTW library
 */

#include <complex.h>
#include <fftw3.h>

#define SIZE 32
#define OMEGA 5

double re(fftw_complex c) { return c[0]; }
double im(fftw_complex c) { return c[1]; }
double mag(fftw_complex c) { return std::sqrt(c[0]*c[0] + c[1]*c[1]); }

int main( void ) {
  double *signal;
  fftw_plan pr2c;

  signal = (double *) fftw_malloc(sizeof(fftw_complex) * (SIZE/2 + 1));

  bzero(signal, sizeof(double) * (SIZE/2 + 1));
  for (int i=0; i<SIZE; i++) {
    double sig = std::sin(2.0 * M_PI * OMEGA * i / SIZE);         // sinewave in bin 5
    double hanning = 0.5 - 0.5 * std::cos(2.0 * M_PI * i / SIZE); // hanning window
    signal[i] = sig * hanning;                                    // windowed sinewave
  }
  
  // plan and execute using in-place signal buffer
  pr2c = fftw_plan_dft_r2c_1d(SIZE, (double *)signal, (fftw_complex *)signal, FFTW_ESTIMATE);
  fftw_execute(pr2c);

  // print the results.  Expect:
  // bin 04 m= 4.000
  // bin 05 m= 8.000
  // bin 06 m= 4.000
  for (int i=0; i<SIZE/2; i++) {
    fftw_complex *fft = (fftw_complex *)signal;
    printf("bin %02d: r=%6.3f i=%6.3f m=%6.3f\n", i, re(fft[i]), im(fft[i]),  mag(fft[i]));
  }

  fftw_destroy_plan(pr2c);
  fftw_free(signal); 

  return 0;
}
