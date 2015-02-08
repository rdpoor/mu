/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  ================================================================
*/
#include "psi_file_generator.h"
#include <strings.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>
#include <FileRead.h>

namespace mu {

  // ================================================================
  // Creating and loading a .psi file.
  //
  // A .psi is the source waveform, precomputed to contain:
  // - the original waveform, reduced to a single channel
  // - the period measured around each sample

  void PsiFileGenerator::analyzeFile(std::string wav_file_name, 
                                     std::string psi_file_name,
                                     double estimated_period) {
    wav_file_name_ = wav_file_name;
    psi_file_name_ = psi_file_name;
    estimated_period_ = estimated_period;

    stk::FileRead wav_file;
    stk::StkFrames tmp_buffer;

    int n_frames;
    int n_channels;
    // TODO: catch StkError here...
    wav_file.open(wav_file_name);
    n_frames = wav_file.fileSize();
    n_channels = wav_file.channels();

    // read entire file
    tmp_buffer.resize(n_frames, n_channels);
    wav_file.read(tmp_buffer);
    wav_file.close();

    // mix down to single channel
    sample_buffer_.resize(n_frames, 1);
    zeroBuffer(sample_buffer_);   // do we need this?
    for (long int frame = 0; frame < n_frames; frame++) {
      for (int channel = 0; channel < n_channels; channel++) {
        sample_buffer_(frame, 0) += tmp_buffer(frame, channel) / n_channels;
      }
    }
    tmp_buffer.resize(0, 0);    // be nice to memory manager

    // generate deltas
    dsample_buffer_.resize(n_frames, 1);
    stk::StkFloat prev = 0.0;
    for (int frame=n_frames-1; frame>=0; frame--) {
      dsample_buffer_[frame] = prev - sample_buffer_[frame];
      prev = sample_buffer_[frame];
    }
    
    // compute periods
    period_buffer_.resize(n_frames, 1);
    computePeriods();
  }

  void PsiFileGenerator::zeroBuffer(stk::StkFrames &b) {
    bzero(&(b[0]), b.frames() * b.channels() * sizeof(stk::StkFloat));
  }

  void PsiFileGenerator::writePsiData() {
    FILE* ofd = fopen(psi_file_name_.c_str(), "w");
    if (NULL == ofd) {
      fprintf(stderr, 
              "Failed to open %s for writing\n", 
              psi_file_name_.c_str());
      return;
    }
    fprintf(ofd, 
            "# PSI v1.1, %f, \"%s\"\n", 
            estimated_period_, 
            wav_file_name_.c_str());

    int n_frames = sample_buffer_.frames();

    fprintf(ofd, "%d\n", n_frames);

    for (long int frame = 0; frame < n_frames; frame++) {
      fprintf(ofd, 
              "%f, %.4f\n",
              sample_buffer_[frame],
              period_buffer_[frame]);
    }
    fprintf(stderr, 
            "Wrote %d items to %s.\n", 
            n_frames, 
            psi_file_name_.c_str());
    fclose(ofd);
  }

  int cmp_double(const void *px, const void *py)
  {
    double x = *(double *)px, y = *(double *)py;
    return (x < y) ? -1 : (x > y) ? 1 : 0;
  }
  
  double PsiFileGenerator::medianPeriod() {
    int n_items = period_buffer_.size();
    int n_bytes = n_items * sizeof(double);

    double *periods = (double *)malloc(n_bytes);
    memcpy(periods, &period_buffer_[0], n_bytes);
    qsort(periods, n_items, sizeof(double), cmp_double);
    double median = periods[n_items/2];
    free(periods);
    return median;
  }

  struct minimization_params { PsiFileGenerator *psi_file_generator; double tau; };

  // function to minimize.  This is just a thin layer to get us into
  // pfg->min_correlation_aux (where all the instance variables are
  // available).
  double min_correlation(double x, void *params) {
    struct minimization_params * p = (struct minimization_params *)params;
    PsiFileGenerator *pfg = p->psi_file_generator;
    double tau = p->tau;
    return pfg->min_correlation_aux(x, tau);
  }

  // TODO: Make tolerance a parameter
#define TOLERANCE 0.2
#define MAX_ITER 10

  void PsiFileGenerator::computePeriods() {
    // set up gsl minimizer
    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;

    T = gsl_min_fminimizer_brent;
    s = gsl_min_fminimizer_alloc(T);
    gsl_set_error_handler_off();

    for (int frame=0; frame < sample_buffer_.frames(); frame++) {
      period_buffer_[frame] = computePeriod(s, frame, false);
    }
    gsl_min_fminimizer_free(s);
  }

  double PsiFileGenerator::computePeriod(void *handle, double tau, bool chatty) {
    gsl_min_fminimizer *s = (gsl_min_fminimizer *)handle;
    struct minimization_params params = { this, tau };

    gsl_function F;
    F.function = &min_correlation;
    F.params = &params;
    double m = estimated_period_;
    double a = estimated_period_ * (1 - TOLERANCE);
    double b = estimated_period_ * (1 + TOLERANCE);
    
    gsl_min_fminimizer_set(s, &F, m, a, b);
    
    int iter = 0;
    int status = GSL_CONTINUE;
    while ((iter < MAX_ITER) && (status == GSL_CONTINUE)) {
      status = gsl_min_fminimizer_iterate(s);
      if (status == GSL_FAILURE) return estimated_period_;
      m = gsl_min_fminimizer_x_minimum(s);
      a = gsl_min_fminimizer_x_lower(s);
      b = gsl_min_fminimizer_x_upper(s);
      if (chatty) {
        printf ("%5d [%.5f, %.5f] %.5f %.5f\n", iter, a, b, m, b - a);
        fflush(stdout);
      }
      status = gsl_min_test_interval(a, b, 0.5, 0.0); // within half of a period
      if (status == GSL_SUCCESS) return m;
      iter++;
    }
    // return best m yet
    return m;
  }

  // Compute discrete autocorrelation at tau + period/2 and
  // tau - period/2.  
  double PsiFileGenerator::min_correlation_aux(double period, double tau) {
    // TODO: Pass window size as a parameter (necessary for upsampled
    // wav files or low notes).
    // fprintf(stderr,"min_correlation_aux: %f, %p, %f\n", period, this, tau);
    double window_length = 1024;
    double tot = 0.0;
    double i0 = tau - (period/2) - (window_length/2);
    double i1 = tau + (period/2) - (window_length/2);

    for (int i = round(window_length)-1; i>=0; i--) {
      tot += getFSample(i0) * getFSample(i1);
      i0 += 1.0;
      i1 += 1.0;
    }
    // fprintf(stderr,"min_correlation_aux: tot = %f\n", tot);
    return -tot;
  }

} // namespace mu

void usage(int ac, char *av[]) {
  fprintf(stderr, 
          "USAGE: %s <wav_file_name> <psi_file_name> <estimated_period>\n",
          av[0]);
  exit(-1);
}

/*
 * SYNOPSIS:
 * 
 * ./psi_file_generator wav_file_name psi_file_name estimated_period
 * Create psi_file as a .csv file with:
 *   sample, period
 * for each sample.  (Warning: these files get large!)
 *
 * ./psi_file_generator wav_file_name - estimated_period
 * Calculate and print on stdout:
 *   wav_file_name, measured_period, estimated_period
 */
int main(int ac, char *av[]) {
  if (ac != 4) usage(ac, av);
  mu::PsiFileGenerator pfg;
  std::string wav_file_name = av[1];
  std::string psi_file_name = av[2];
  double estimated_period = atof(av[3]);

  pfg.analyzeFile(wav_file_name,
                  psi_file_name,
                  estimated_period);
  if (psi_file_name.compare("-") == 0) {
    // write filename, median_period, estimated_period
    printf("%s, %f, %f\n", wav_file_name.c_str(), pfg.medianPeriod(), estimated_period);
  } else {
    // write the results to a .psi file
    pfg.writePsiData();
  }
}
