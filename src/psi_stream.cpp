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
#include "psi_stream.h"
#include <strings.h>
#include <math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_min.h>

namespace mu {

  // static const double kTwoPi = M_PI * 2.0;

  PsiStream::PsiStream()
    : estimated_period_ ( 100.0 ),
      sample_source_ ( NULL ), 
      tau_source_ ( NULL ),
      omega_source_ ( NULL ),
      expected_tick_ ( 0 ),
      needs_setup_ ( true ),
      phi_ ( 0.0 ) {
    temp_buffer_.resize(0, 1);
    sample_buffer_.resize(0, 1);
    dsample_buffer_.resize(0, 1);
    period_buffer_.resize(0, 1);
    tau_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    omega_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  PsiStream::~PsiStream() {
  }

  void PsiStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "SampleSource" << std::endl;
    ss << sample_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "TauSource" << std::endl;
    ss << tau_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "OmegaSource" << std::endl;
    ss << omega_source_->inspect(level+1);
  }

  PsiStream& PsiStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    zeroBuffer(buffer);
    if ((sample_source_ == NULL) || 
        (tau_source_ == NULL) || 
        (omega_source_ == NULL)) {
      return *this;
    }

    // Fetch sample_buffer_ (and other initialization)  as needed
    if (needs_setup_) {
      setup(player);
    }

    tau_buffer_.resize(buffer.frames(), 1);
    tau_source_->step(tau_buffer_, tick, player);
    omega_buffer_.resize(buffer.frames(), 1);
    omega_source_->step(omega_buffer_, tick, player);

    if (tick != expected_tick_) {
      // time has skipped
      phi_ = tau_buffer_[0];
    }
    expected_tick_ = tick + buffer.frames();

    for (int frame=0; frame<tau_buffer_.frames(); frame++) {
      stk::StkFloat tau = tau_buffer_[frame];
      stk::StkFloat omega = omega_buffer_[frame];
      
      stk::StkFloat sample = generatePhi(tau, omega);
              
      for (int channel = 0; channel < buffer.channels(); channel++) {
        buffer(frame, channel) = sample;
      }
    }
    return *this;
  }

  // TODO: guarantee that getPeriod(tau) always returns a positive
  // value (or else we're stuck in a loop).
  // TODO: is there a closed form that will replace these two loops?
  stk::StkFloat PsiStream::generatePhi(stk::StkFloat tau, stk::StkFloat omega) {

    // Assure that phi <= tau and within one period
    stk::StkFloat period = getPeriod(tau);
    while (phi_ > tau) phi_ -= period;
    while (phi_ < (tau - period)) phi_ += period;
    
    // Get the samples surrounding tau at phi_ and one period later.
    stk::StkFloat s0 = getFSample(phi_);
    stk::StkFloat s1 = getFSample(phi_ + period);
    
    // Interpolate according to tau's distance from phi_
    stk::StkFloat alpha = (tau - phi_) / period;
    
    // Update phi
    phi_ += omega;

    // return the interpolated sample
    return s0 + (s1 - s0) * alpha;
  }

  void PsiStream::setup(Player& player) {
    if (!needs_setup_) return;

    // Fetch waveform into a dedicated buffer
    int n_frames = sample_source_->getEnd() - sample_source_->getStart();

    // TODO: this is really bad.  we're assuming that source is stero...
    temp_buffer_.resize(n_frames, 2);
    sample_source_->step(temp_buffer_, 0, player);

    // convert stereo samples in temp_buffer_ to mono samples in
    // sample_buffer_.
    sample_buffer_.resize(n_frames, 1);
    for (int frame = 0; frame < n_frames; frame++) {
      sample_buffer_[frame] = 
        (temp_buffer_(frame, 0) + temp_buffer_(frame, 1)) * 0.707;
    }
    
    // Compute deltas
    dsample_buffer_.resize(n_frames, 1);
    stk::StkFloat prev = 0.0;
    for (int frame=sample_buffer_.frames()-1; frame>=0; frame--) {
      dsample_buffer_[frame] = prev - sample_buffer_[frame];
      prev = sample_buffer_[frame];
    }

    // Compute periods
    period_buffer_.resize(n_frames, 1);
    computePeriods();

    needs_setup_ = false;
  }

  // Get the period around t=tau in the waveform.  Assumes
  // computePeriod() has been called to setup period_buffer_.
  stk::StkFloat PsiStream::getPeriod(stk::StkFloat tau) {
    int n_frames = period_buffer_.frames();
    if (tau <= 0) {
      return period_buffer_[0];
    } else if (tau >= n_frames-1) {
      return period_buffer_[n_frames-1];
    } else {
      long int i = (long int)tau;
      double alpha = tau - i;
      stk::StkFloat p0 = period_buffer_[i];
      stk::StkFloat p1 = period_buffer_[i+1];
      return p0 + alpha * (p1 - p0);
    }
  }

#if 1

  struct minimization_params { PsiStream *psi_stream; double tau; };

  // function to minimize
  double function_to_minimize(double x, void *params) {
    struct minimization_params * p = (struct minimization_params *)params;
    PsiStream *ps = p->psi_stream;
    double tau = p->tau;
    return ps->min_correlation(x, tau);
  }

#define TOLERANCE 0.2
#define MAX_ITER 10

  // debugging hack: make a pseudo-csv for minimiazation fn
  void PsiStream::cough(double tau) {

    // print out samples from tau - estimated_period_ to tau + estimated_period_
    printf("# samples\n");
    for (int i=tau-estimated_period_; i<tau+estimated_period_; i++) {
      printf("%d, %f\n", i, getFSample(i));
    }

    double a = estimated_period_ * (1 - TOLERANCE);
    double b = estimated_period_ * (1 + TOLERANCE);
    printf("# tau = %f\n", tau);
    for (int i=a; i<b; i++) {
      printf("%d, %f\n", i, min_correlation(i, tau));
    }
    fflush(stdout);

  }
    
  void PsiStream::computePeriods() {
    // set up gsl minimizer
    const gsl_min_fminimizer_type *T;
    gsl_min_fminimizer *s;

    T = gsl_min_fminimizer_brent;
    s = gsl_min_fminimizer_alloc(T);
    gsl_set_error_handler_off();

    for (int frame=0; frame < sample_buffer_.frames(); frame++) {
      period_buffer_[frame] = computePeriod(s, frame, false);
      // printf("%d, %f\n", frame, period_buffer_[frame]);
    }
    // fflush(stdout);

    gsl_min_fminimizer_free(s);
  }

  // The void * thing is to avoid exposing the gsl datatypes into the
  // .h file (which would require all subscribers to include the gsl
  // files as well).  There's probably a better way.
  double PsiStream::computePeriod(void *handle, double tau, bool chatty) {
    gsl_min_fminimizer *s = (gsl_min_fminimizer *)handle;
    struct minimization_params params = { this, tau };

    gsl_function F;
    F.function = &function_to_minimize;
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

  double PsiStream::min_correlation(double period, double tau) {
    // Compute discrete autocorrelation at tau+period/2 and
    // tau - period/2.  
    //
    // NOTE: Making the window length a multiple of the estimated
    // period leads to ripple on the period signal.  Make it fixed
    // and it works MUCH better.
    double window_length = 1024; /* 2*period; */
    double tot = 0.0;
    double i0 = tau - (period/2) - (window_length/2);
    double i1 = tau + (period/2) - (window_length/2);

    for (int i = round(window_length)-1; i>=0; i--) {
      tot += getFSample(i0) * getFSample(i1);
      i0 += 1.0;
      i1 += 1.0;
    }
    return -tot;
  }

#else
  // ================================================================
  // workable, but far from optimal.  which is why I switched to the
  // GSL minimizers

  void PsiStream::computePeriods() {
    for (int frame=sample_buffer_.frames()-1; frame>=0; frame--) {
      period_buffer_[frame] = computePeriod(frame);
    }
  }

  typedef struct {
    double index;
    double value;
  } iv_pair;
  
  void swap_iv_pairs(iv_pair &a, iv_pair &b) {
    double tmp_v = a.value;
    int tmp_i = a.index;
    a.value = b.value;
    a.index = b.index;
    b.value = tmp_v;
    b.index = tmp_i;
  }

  void sort_iv_pairs(iv_pair pairs[]) {
    if (pairs[0].value > pairs[1].value) swap_iv_pairs(pairs[0], pairs[1]);
    if (pairs[1].value > pairs[2].value) swap_iv_pairs(pairs[1], pairs[2]);
    if (pairs[0].value > pairs[1].value) swap_iv_pairs(pairs[0], pairs[1]);
  }
  
  void print_pairs(iv_pair a[]) {
    for (int i=0; i<3; i++) {
      printf("[%f %f] ", a[i].index, a[i].value);
    }
    printf("\n");
  }

#define TOLERANCE 0.2

  stk::StkFloat PsiStream::computePeriod(Tick frame) {
    // return estimated_period_;

    int max_passes = 7;
    iv_pair pairs[3];

    pairs[0].index = estimated_period_ * (1 - TOLERANCE);
    pairs[1].index = estimated_period_ * (1 + TOLERANCE);
    pairs[2].index = estimated_period_;
    pairs[2].value = correlate(frame, pairs[2].index);

    // At each pass, keep the best value and halve the distance
    // to it from the other two.
    for (int pass = 0; pass < max_passes; pass++) {
      // here, pairs[2] has the highest value.  keep that pair
      // and move the other two pairs halfway towards that.
      pairs[0].index = (pairs[0].index + pairs[2].index)/2.0;
      pairs[0].value = correlate(frame, pairs[0].index);
      pairs[1].index = (pairs[1].index + pairs[2].index)/2.0;
      pairs[1].value = correlate(frame, pairs[1].index);

      sort_iv_pairs(pairs);
      if (frame % 1000 == 0) {
        printf("tau = %ld pass = %d: ", frame, pass); 
        print_pairs(pairs); 
        fflush(stdout);
      }
    }
    return pairs[2].index;
  }

  // Compute discrete autocorrelation at tau+period/2 and
  // tau - period/2.  Use 2*period for window size.
  stk::StkFloat PsiStream::correlate(double tau, double period) {
    stk::StkFloat tot = 0.0;
    stk::StkFloat i0 = tau - (period/2) - period;
    stk::StkFloat i1 = tau + (period/2) - period;
    for (int i = round(2*period)-1; i>=0; i--) {
      tot += getFSample(i0) * getFSample(i1);
      i0 += 1.0;
      i1 += 1.0;
    }
    return tot;
  }
#endif


}


