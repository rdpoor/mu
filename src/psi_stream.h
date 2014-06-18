/*
 * PsiStream: Phase Synchronous Interpolation.
 *
 * PsiStream gives independent control of the timing and pitch of a
 * quasi-periodic waveform.  It takes two Stream inputs:
 *
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
 * - signalFileName is the waveform to be played back.
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
#include "psi_waveform.h"

namespace mu {

  class PsiStream : public Stream {
  public:

    // static const stk::StkFloat kDefaultPeriod = 100.0;
    // static const stk::StkFloat kDefaultNPeriods = 100;

    PsiStream( void );
    ~PsiStream( void );

    std::string getClassName() { return "PsiStream"; }
    virtual void inspectAux(std::stringstream& ss, int level);

    void step(stk::StkFrames& buffer, Tick tick, Player &player);

    // assumes that setPsiFileName() has been called...
    Tick getFrameCount() { return psi_waveform_->getFrameCount(); }

    std::string getPsiFileName( void ) { return psi_waveform_->getFileName(); }
    PsiStream& setPsiFileName( std::string psi_file_name );

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

  protected:
    PsiWaveform *psi_waveform_;
    Stream *tau_source_;
    Stream *omega_source_;
    Tick expected_tick_;        // next tick time (if time is contiguous)
    stk::StkFloat phi_;
    stk::StkFrames tau_buffer_;
    stk::StkFrames omega_buffer_;

  private:

    stk::StkFloat generateSample(stk::StkFloat tau, stk::StkFloat omega);

  };                            // class PsiStream
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
