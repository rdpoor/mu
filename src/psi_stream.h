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

/*
 * PsiStream: Phase Synchronous Interpolation.
 *
 * PsiStream gives independent control of the timing and pitch of a
 * quasi-periodic waveform.  It takes two stream inputs:
 *
 * - tauSource controls the timing of the playback.
 * - omegaSource controls the pitch of the playback.
 *
 * tauSource is a MuStream whose instantaneous value determines the timing of
 * the waveform: 0 is the start of the waveform, length(waveform)-1 is the end
 * of the waveform.  omegaSource is a MuStream whose instantaneous value
 * determines relative frequency of the sample to be played back: 1.0 is normal
 * pitch, 2.0 is up one octave, 0.5 is down one octave.
 *
 * In addition, PsiStream has one instance variable:
 * - psi_waveform_: the waveform to be played back.
 *
 * How it works: 
 *
 * Given:
 *   t = current frame count
 *   tau(t) = desired time in the waveform to be played back
 *   omega(t) = relative frequency to be played
 *
 *   s(t) = one sample of the original waveform at frame t
 *   p(t) = period measured at frame t
 *
 * PsiStream maintains a pointer, phi, that is always less than tau and always
 * within one period of tau.  To generate a sample, PsiStream performs a 2D
 * linear interpolation between the previous period and the current period 
 * surrounding tau:
 * 
 *   q0 = s(phi) [linearly interpolated]
 *   q1 = s(phi + p(phi_))  [linearly interpolated]
 *  
 * Then q0 and q1 are blended according to the delta between tau and phi.
 * 
 * After emitting each sample, phi is incremented by omega.  If phi lags
 * more than one frame behind tau or overtakes tau, it is adjusted to 
 * be less than tau and within one period of tau.
 *
 * Notes: 
 *
 * At initialization time, PsiStream computes Period(i), where
 * Period(i) is the period at the i'th frame of signalSource.  This
 * can take a while.
 *
 * If tau jumps by a significant amount, it will take linear time for
 * phi to renormalize.  (A future version may avoid this limitation.)
 */

#ifndef MU_PSI_STREAM_H
#define MU_PSI_STREAM_H

#include "mu_types.h"
#include "mu_stream.h"
#include "psi_waveform.h"

namespace mu {

  class PsiStream : public MuStream {
  public:

    PsiStream( void );
    virtual ~PsiStream( void );
    virtual PsiStream *clone();

    // assumes that setPsiFileName() has been called...
    MuTick frame_count() { return psi_waveform_->frame_count(); }

    std::string psi_filename( void ) { return psi_waveform_->file_name(); }
    void set_psi_filename( std::string psi_file_name );

    MuStream *tau_source( void ) { return tau_source_; }
    void set_tau_source(MuStream *source) { tau_source_ = source; }

    MuStream *omega_source( void ) { return omega_source_; }
    void set_omega_source(MuStream *source) { omega_source_ = source; }

    std::string get_class_name() { return "PsiStream"; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

  protected:
    void inspect_aux(int level, std::stringstream *ss);

    PsiWaveform *psi_waveform_;
    MuStream *tau_source_;
    MuStream *omega_source_;
    MuFloat phi_;
    stk::StkFrames tau_buffer_;
    stk::StkFrames omega_buffer_;

  private:

    MuFloat generate_sample(MuFloat tau, MuFloat omega);

  };                            // class PsiStream
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
