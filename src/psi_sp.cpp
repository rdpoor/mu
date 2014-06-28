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
#include "psi_sp.h"
#include <math.h>
#include <stdio.h>
#include <strings.h>

namespace mu {

  // static const double kTwoPi = M_PI * 2.0;

  PsiSP::PsiSP()
    : tau_source_ ( NULL ),
      omega_source_ ( NULL ),
      expected_tick_ ( 0 ),
      phi_ ( 0.0 ) {
    tau_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    omega_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  PsiSP::~PsiSP() {
  }

  void PsiSP::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "TauSource" << std::endl;
    ss << tau_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "OmegaSource" << std::endl;
    ss << omega_source_->inspect(level+1);
  }

  void PsiSP::step(stk::StkFrames& buffer, Tick tick, Player& player) {
    zeroBuffer(buffer);
    if ((tau_source_ == NULL) || 
        (omega_source_ == NULL)) {
      return;
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

    if (buffer.channels() == 1) {
      for (int frame=0; frame<tau_buffer_.frames(); frame++) {
        stk::StkFloat tau = tau_buffer_[frame];
        stk::StkFloat omega = omega_buffer_[frame];
        stk::StkFloat sample = generateSample(tau, omega);
        buffer(frame, 0) = sample;
      }
    } else if (buffer.channels() == 2) {
      for (int frame=0; frame<tau_buffer_.frames(); frame++) {
        stk::StkFloat tau = tau_buffer_[frame];
        stk::StkFloat omega = omega_buffer_[frame];
        stk::StkFloat sample = generateSample(tau, omega);
        buffer(frame, 0) = sample;
        buffer(frame, 1) = sample;
      }
    } else {
      for (int frame=0; frame<tau_buffer_.frames(); frame++) {
        stk::StkFloat tau = tau_buffer_[frame];
        stk::StkFloat omega = omega_buffer_[frame];
        stk::StkFloat sample = generateSample(tau, omega);
        for (int channel = 0; channel < buffer.channels(); channel++) {
          buffer(frame, channel) = sample;
        }
      }
    }

    return;
  }

  PsiSP& PsiSP::setPsiFileName( std::string psi_file_name ) {
    psi_waveform_ = PsiWaveform::lookup(psi_file_name);
    return *this;
  }

  // TODO: guarantee that getPeriod(tau) always returns a positive
  // value (or else we're stuck in a loop).
  // TODO: is there a closed form that will replace these two loops?
  stk::StkFloat PsiSP::generateSample(stk::StkFloat tau, stk::StkFloat omega) {

    // Assure that phi <= tau and within one period
    stk::StkFloat period = psi_waveform_->getPeriod(tau);
    while (phi_ > tau) phi_ -= period;
    while (phi_ < (tau - period)) phi_ += period;
    
    // Get the samples surrounding tau at phi_ and one period later.
    stk::StkFloat s0 = psi_waveform_->getFSample(phi_);
    stk::StkFloat s1 = psi_waveform_->getFSample(phi_ + period);
    
    // Interpolate according to tau's distance from phi_
    stk::StkFloat alpha = (tau - phi_) / period;
    
    // Update phi
    phi_ += omega;

    // return the interpolated sample
    return s0 + (s1 - s0) * alpha;
  }

}


