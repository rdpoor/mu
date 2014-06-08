/*
 * PsiStream: Phase Synchronous Interpolation.
 *
 * PsiStream gives independent control of the timing and pitch of a
 * quasi-periodic waveform.  It takes three Stream inputs:
 *
 * - signalSource is the waveform to be played back.
 * - tauSource controls the timing of the playback.
 * - omegaSource controls the pitch of the playback.
 *
 * tauSource is a Stream whose instantaneous value determines where in
 * the waveform is to be played back: 0 is the start of the waveform,
 * length(waveform)-1 is the end of the waveform.  omegaSource is a
 * stream whose instantaneous value determines pitch of the sample to
 * be played back: 1.0 is normal pitch, 2.0 is up one octave, 0.5 is
 * down one octave.
 *
 * In addition, PsiStream has one instance variable:
 * - estimatedPeriod: the period (in samples) of signalSource.
 *
 * How it works: 
 *
 * During playback, tau indicates the desired point in the waveform to
 * be played back.  PsiStream maintains a pointer, phi, that is always
 * less than tau and always within one period of tau.  To generate a
 * sample, PsiStream interpolates between the period previous to tau
 * and the period following tau:
 *
 * Given:
 *   S(t) = one sample of the original waveform at sample t
 *   P(t) = period measured at sample t
 * Then:
 *   psi(t) = S(phi) + (S(phi+P(t)) - S(phi)) * (tau - phi) / P(t);
 * 
 * After emitting each sample, phi is incremented by omega.  If phi lags
 * more than one sample behind tau or overtakes tau, it is adjusted to 
 * be less than tau and within one period of tau.
 *
 * Notes: 
 *
 * At initialization time, PsiStream computes Period(i), where
 * Period(i) is the period at the i'th sample of signalSource.  This
 * can take a while.
 *
 * If tau jumps by a significant amount, it will take linear time for
 * phi to renormalize.  (A future version may avoid this limitation.)
 */

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

#ifndef MU_PSI_STREAM_H
#define MU_PSI_STREAM_H

#include "mu.h"
#include "stream.h"

namespace mu {

  class PsiStream : public Stream {
  public:

    // static const stk::StkFloat kDefaultPeriod = 100.0;
    // static const stk::StkFloat kDefaultNPeriods = 100;

    PsiStream( void );
    ~PsiStream( void );

    std::string getClassName() { return "PsiStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    PsiStream& step(stk::StkFrames& buffer, Tick tick, Player &player);

    double getEstimatedPeriod( void ) { return estimated_period_; }
    PsiStream& setEstimatedPeriod( double estimated_period ) { 
      estimated_period_ = estimated_period; 
      return *this; 
    }

    Stream *getSampleSource( void ) { return sample_source_; }
    PsiStream& setSampleSource(Stream *source) { 
      sample_source_ = source; 
      needs_setup_ = true;
      return *this; 
    }

    Stream *getTauSource( void ) { return tau_source_; }
    PsiStream& setTauSource(Stream *source) { 
      tau_source_ = source; 
      return *this; 
    }

    Stream *getOmegaSource( void ) { return omega_source_; }
    PsiStream& setOmegaSource(Stream *source) { 
      omega_source_ = source; 
      return *this; 
    }

    // Precompute periods, etc.  Calling this avoids doing the heavy computation
    // while the stream is running.  Can only (meaningfully) be called after the
    // sampleSource is completely available.
    void setup(Player& player);

    // Not public, but needs to be visible as a callback to the GSL
    // minimization library.
    double min_correlation(double period, double tau);

  protected:
    double estimated_period_;
    Stream *sample_source_;
    Stream *tau_source_;
    Stream *omega_source_;
    Tick expected_tick_;        // next tick time (if time is contiguous)
    bool needs_setup_;          // true if one-time setup() needed
    stk::StkFloat phi_;
    stk::StkFrames temp_buffer_; // used for stero to mono conversion
    stk::StkFrames sample_buffer_;
    stk::StkFrames dsample_buffer_; // forward difference of sample_buffer_ 
    stk::StkFrames period_buffer_;  // period of sample_buffer_
    stk::StkFrames tau_buffer_;
    stk::StkFrames omega_buffer_;

  private:

    // Fetch the ith' sample from sample_buffer_, returning 0
    // if i is out of range.
    stk::StkFloat getSample( long i ) {
      if ((i < 0) || (i>=sample_buffer_.frames())) {
        return 0.0;
      } else {
        return sample_buffer_[i];
      }
    }

    // Fetch the t'th sample from sample_buffer_ using linear
    // interpolation for any fractional part of t.  Returns 0.0 if t
    // is out of range.  Assumes:
    // 
    // dsample_buffer_[i] = sample_buffer[i+1]-sample_buffer[i];
    stk::StkFloat getFSample( double t ) {
      if ((t < 0) || (t >= sample_buffer_.frames())) {
        return 0.0;
      } else {
        long int i = (long int)t;
        double alpha = t - i;
        return sample_buffer_[i] + alpha * dsample_buffer_[i];
      }
    }

    stk::StkFloat generatePhi(stk::StkFloat tau, stk::StkFloat omega);
    stk::StkFloat getPeriod(stk::StkFloat tau);
    void cough(double tau);     // debugging only
    void computePeriods();
    double computePeriod(void *handle, double tau, bool chatty);
    // stk::StkFloat correlate(double tau, double period);

  };                            // class PsiStream
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
