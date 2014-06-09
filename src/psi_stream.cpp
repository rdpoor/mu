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
#include <math.h>
#include <stdio.h>
#include <strings.h>

namespace mu {

  // static const double kTwoPi = M_PI * 2.0;

  PsiStream::PsiStream()
    : tau_source_ ( NULL ),
      omega_source_ ( NULL ),
      expected_tick_ ( 0 ),
      phi_ ( 0.0 ) {
    sample_buffer_.resize(0, 1);
    dsample_buffer_.resize(0, 1);
    period_buffer_.resize(0, 1);
    tau_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    omega_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  PsiStream::~PsiStream() {
  }

  void PsiStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "TauSource" << std::endl;
    ss << tau_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "OmegaSource" << std::endl;
    ss << omega_source_->inspect(level+1);
  }

  PsiStream& PsiStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    zeroBuffer(buffer);
    if ((tau_source_ == NULL) || 
        (omega_source_ == NULL)) {
      return *this;
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
      
      stk::StkFloat sample = generateSample(tau, omega);
              
      for (int channel = 0; channel < buffer.channels(); channel++) {
        buffer(frame, channel) = sample;
      }
    }
    return *this;
  }

  PsiStream& PsiStream::setPsiFileName( std::string psi_file_name ) {
      psi_file_name_ = psi_file_name;
      readPsiFile();
      return *this;
  }

  // TODO: guarantee that getPeriod(tau) always returns a positive
  // value (or else we're stuck in a loop).
  // TODO: is there a closed form that will replace these two loops?
  stk::StkFloat PsiStream::generateSample(stk::StkFloat tau, stk::StkFloat omega) {

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

  void PsiStream::readPsiFile() {
    int n_items_read, n_lines_read, n_frames;
    double x, y, z;
    char *line = NULL;
    size_t linecap = 0;

    FILE* ifd = fopen(psi_file_name_.c_str(), "r");
    if (NULL == ifd) {
      // TODO: need to actually handle the error somehow...
      fprintf(stderr, "Failed to open %s for reading.\n", psi_file_name_.c_str());
      return;
    }

    if (getline(&line, &linecap, ifd) == EOF) {
      fprintf(stderr, "Unexpected EOF while searching for version #.\n");
      goto err;
    }
      
    if (getline(&line, &linecap, ifd) == EOF) {
      fprintf(stderr, "Unexpected EOF while searching for frame count.\n");
      goto err;
    }

    n_items_read = sscanf(line, "%d\n", &n_frames);
    if (n_items_read != 1) {
      fprintf(stderr, "Format error trying to read frame count.\n");
      goto err;
    }

    sample_buffer_.resize(n_frames, 1);
    dsample_buffer_.resize(n_frames, 1);
    period_buffer_.resize(n_frames, 1);

    n_lines_read = 0;
    for (int i=0; i<n_frames; i++) {
      if (getline(&line, &linecap, ifd) == EOF) {
        fprintf(stderr, "Premature end of file.  Read %d out of %d.\n", n_lines_read, n_frames);
        goto err;
      }
      n_items_read = sscanf(line, "%la, %la, %la\n", &x, &y, &z);
      if (n_items_read != 3) {
        fprintf(stderr, "at line %d: unrecognized format: %s\n", n_lines_read, line);
        goto err;
      }
      sample_buffer_[i] = x;
      dsample_buffer_[i] = y;
      period_buffer_[i] = z;
      n_lines_read += 1;
    }

    fprintf(stderr, "Read %d frames from %s\n", n_frames, psi_file_name_.c_str());
  err:
    fclose(ifd);
  }

}


