/*
  ================================================================
  Copyright (C) 2014 Robert D. Poor
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  ================================================================
*/
#include "psi_stream.h"
#include "mu_utils.h"
#include <math.h>
#include <stdio.h>
#include <strings.h>

namespace mu {

  // static const double kTwoPi = M_PI * 2.0;

  PsiStream::PsiStream()
    : tau_source_ ( NULL ),
      omega_source_ ( NULL ),
      phi_ ( 0.0 ) {
    tau_buffer_.resize(stk::RT_BUFFER_SIZE, 1);
    omega_buffer_.resize(stk::RT_BUFFER_SIZE, 1);
  }

  PsiStream::~PsiStream() {
    if (tau_source_) { delete tau_source_; }
    if (omega_source_) { delete omega_source_; }
  }

  // TODO: make sure psi_waveform copies properly
  PsiStream *PsiStream::clone() {
    PsiStream *c = new PsiStream();
    c->set_psi_filename(psi_filename());
    c->set_tau_source(MuUtils::clone_stream(tau_source_));
    c->set_omega_source(MuUtils::clone_stream(omega_source_));
    return c;
  }

  void PsiStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss); *ss << "tau_source" << std::endl;
    tau_source_->inspect_aux(level+1, ss);
    inspect_indent(level, ss); *ss << "omega_source" << std::endl;
    omega_source_->inspect_aux(level+1, ss);
  }

  bool PsiStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if ((tau_source_ == NULL) || (omega_source_ == NULL)) {
      return false;
    }

    int n_frames = buffer->frames();
    int n_channels = buffer->channels();

    MuUtils::zero_buffer(buffer);

    // TODO: What if tau_source_->render() or omega_source_->render() returns false?
    tau_buffer_.resize(n_frames, 1);
    tau_source_->render(buffer_start, &tau_buffer_);

    omega_buffer_.resize(n_frames, 1);
    omega_source_->render(buffer_start, &omega_buffer_);

    if (buffer_start == 0) {
      // restart
      phi_ = tau_buffer_[0];
    }

    MuFloat sample;

    if (n_channels == 1) {
      for (int frame=0; frame<n_frames; ++frame) {
        MuTick tick = frame + buffer_start;
        if (tick >= 0) {
          sample = generate_sample(tau_buffer_[frame], omega_buffer_[frame]);
        } else {
          sample = 0;
        }
        (*buffer)(frame, 0) = sample;
      }
    } else if (n_channels == 2) {
      for (int frame=0; frame<n_frames; ++frame) {
        MuTick tick = frame + buffer_start;
        if (tick >= 0) {
          sample = generate_sample(tau_buffer_[frame], omega_buffer_[frame]);
        } else {
          sample = 0;
        }
        (*buffer)(frame, 0) = sample;
        (*buffer)(frame, 1) = sample;
      }
    } else {
      for (int frame=0; frame<n_frames; ++frame) {
        MuTick tick = frame + buffer_start;
        if (tick >= 0) {
          sample = generate_sample(tau_buffer_[frame], omega_buffer_[frame]);
        } else {
          sample = 0;
        }
        for (int channel = n_channels-1; channel >= 0; --channel) {
          (*buffer)(frame, channel) = sample;
        }
      }
    }

    return true;
  }

  void PsiStream::set_psi_filename( std::string psi_filename ) {
    psi_waveform_ = PsiWaveform::lookup(psi_filename);
  }

  // TODO: guarantee that getPeriod(tau) always returns a positive
  // value (or else we're stuck in a loop).
  // TODO: is there a closed form that will replace these two loops?
  MuFloat PsiStream::generate_sample(MuFloat tau, MuFloat omega) {

    // Assure that phi <= tau and within one period
    MuFloat period = psi_waveform_->get_period(tau);
    while (phi_ > tau) phi_ -= period;
    while (phi_ < (tau - period)) phi_ += period;
    
    // Get the samples surrounding tau at phi_ and one period later.
    MuFloat s0 = psi_waveform_->get_sample(phi_);
    MuFloat s1 = psi_waveform_->get_sample(phi_ + period);
    
    // Interpolate according to tau's distance from phi_
    MuFloat alpha = (tau - phi_) / period;
    
    // Update phi
    phi_ += omega;

    // return the interpolated sample
    return s0 + (s1 - s0) * alpha;
  }

}


