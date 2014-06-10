/*
 * Test SolaStream
 */

#include "crop_stream.h"
#include "file_read_stream.h"
#include "linseg_stream.h"
#include "loop_stream.h"
#include "mu.h"
#include "rt_player.h"

#define SOUND_DIR "/Users/r/Projects/Mu/SoundSets/A/60.wav"

// TODO: FileReadStream should handle this without a CropSteram
mu::Stream *getSoundFile(std::string file_name) {
  mu::FileReadStream *frs = &((new mu::FileReadStream())->fileName(file_name).doNormalize(true));
  mu::CropStream *cs = &(new mu::CropStream())->setSource(frs).setStart(0);
  return cs;
}

class SolaStream : public mu::Stream {
public:

  SolaStream( void );
  ~SolaStream( void );

  std::string getClassName() { return "SolaStream"; }
  virtual void inspectAux(std::stringstream& ss, int level);
  
  SolaStream& step(stk::StkFrames& buffer, Tick tick, Player &player) {
    zeroBuffer(buffer);
    if ((sample_source_ == NULL) || (phi_source_ == NULL) || (tau_source_ == NULL)) {
      return *this;
    }

    // update inputs
    phi_buffer_.resize(buffer.frames(), buffer.channels());
    phi_source->step(phi_buffer_, tick, player);
    tau_buffer_.resize(buffer.frames(), buffer.channels());
    tau_source->step(tau_buffer_, tick, player);

    for (int i=0; i<buffer.frames(); i++) {
      stk::StkFloat tau = tau_buffer_(i, 0);
      stk::StkFloat phi = phi_buffer_(i, 0);
      // As written, emit() needs to modify phi.  But we want to pass
      // absolute phase as an input.  Timeout to rethink this...
      #####
      stk::StkFloat v = emit(tau, phi);
      for (j=0; j<buffer.channels(); j++) { 
        buffer(i,j) = v; 
      }
    }

  }


  Stream *getSampleSource( void ) { return sample_source_; }
  SolaStream& setSampleSource(Stream *source) { sample_source_ = source; return *this; }

  Stream *getPhiSource( void ) { return phi_source_; }
  SolaStream& setPhiSource(Stream *source) { phi_source_ = source; return *this; }

  Stream *getTauSource( void ) { return tau_source_; }
  SolaStream& setTauSource(Stream *source) { tau_source_ = source; return *this; }

  long int getEstimatedPeriod( void ) { return estimated_period_; }
  SolaStream& setEstimatedPeriod(long int estimated_period) { 
    estimated_period_ = estimated_period; 
    return *this; 
  }

  long int getWindowSize( void ) { return window_size_; }
  SolaStream& setWindowSize(long int window_size) { window_size_ = window_size; return *this; }

protected:
  mu::Stream *sample_source_;
  mu::Stream *phi_source_;
  mu::Stream *tau_source_;
  stk::StkFloat estimated_period_;
  long int window_size_;  

  stk::StkFrames sample_buffer; // pre-fetched samples
  long int[] prev_indeces_;     // relative index to move to prev period
  long int[] next_indeces_;     // relative index to move to next period
  stk::StkFloat[] window_;      // Bartlett Window

private:

  // ================================================================
  // return a Bartlett (triangular) window, window_size_ samples wide.
  stk::StkFloat[] getWindow() {
    if (window_ == NULL) {
      window_ = new stk::StkFloat[window_size_];
      long int half_size = floor(window_size_/2);
      for (i=0; i<=half_size; i++) {
        window[window_size_ - i - 1] = window[i] = (stk::StkFloat)i / half_size;
      }
    }
    return window_;
  }

  // ================================================================
  // Emit one sample based on immediate values of tau and phi
  stk::StkFloat emit(stk::StkFloat tau, stk::StkFloat phi) {
    if (tau < phi) {
      while (true) {
        if (phi < 1) return 0.0;
        phi_prime = phi + phi_prev(phi);
        if (phi_prev <= tau) break;
        phi = phi_prime;        // step back one period
      }
      return intaupolate(tau, phi, phi_prime);
      
    } else if (tau > phi) {
      while (true) {
        if (phi > sample_buffer_.frames() - 1) return 0.0;
        phi_prime = phi + phi_next(phi);
        if (phi_prev >= tau) break;
        phi = phi_prime;        // step forwared one period
      }
      return intaupolate(tau, phi, phi_prime);

    } else /* tau == phi */ {
      return fetchSample(phi);

    }
  }

  // ================================================================
  // Return the number of frames to the period prior to phi.  Use the
  // cached value if it exists, compute it otherwise.
  //
  stk::StkFloat prev_period(stk::StkFloat phi) {
    if ((phi < estimated_period_) || (phi > sample_buffer_.frames())) {
      return -estimated_period;
    }

    long int i_phi = floor(phi);
    if (prev_indeces_(i_phi) == 0) {
      prev_indeces_(i_phi) = compute_period(phi, -estimated_period);
    }
    return prev_indeces_(i_phi);
  }

  // ================================================================
  // Return the number of frames to the period following phi.  Use the
  // cached value if it exists, compute it otherwise.
  //
  stk::StkFloat next_period(stk::StkFloat phi) {
    if ((phi < 0) || (phi > sample_buffer_.frames() - estimated_period)) {
      return estimated_period;
    }

    long int i_phi = floor(phi);
    if (next_indeces_(i_phi) == 0) {
      next_indeces_(i_phi) = compute_period(phi, estimated_period);
    }
    return next_indeces_(i_phi);
  }

  // ================================================================
  // Blend a sample between signal(phi) and signal(phi_prime),
  // depending on the relative value of tau.
  stk::StkFloat intaupolate(stk::StkFloat tau, stk::StkFloat phi, stk::StkFloat phi_prime) {
    stk::StkFloat s0 = fetchSample(phi);
    stk::StkFloat s1 = fetchSample(phi_prime);
    stk::StkFloat alpha = (tau - phi_prime) / (phi - phi_prime);
    return s0 + alpha * (s1 - s0);
  }

  // ================================================================
  // fetch an interpolated sample from sample_buffer_, returning zero
  // if phi exceeds bounds.
  stk::StkFloat fetchSample(stk::StkFloat phi) {
    long int i_phi;

    if ((phi < 0) || (phi > sample_buffer.frames() - 1)) {
      return 0.0;
    } else if ((i_phi = floor(phi)) == phi) {
      return sample_buffer_(i_phi);
    } else {
      stk::StkFloat alpha = phi - i_phi;
      stk::StkFloat s0 = sample_buffer_(i_phi);
      stk::StkFloat s1 = sample_buffer_(i_phi+1);
      return s0 + alpha * (s1 - s0);
    }
  }

  // ================================================================
  // ================================================================
  // ANALYTICAL METHODS START HERE

  // ================================================================
  // Use cross-correlation to find sample offset most similar to the
  // period surrounding phi.

  stk::StkFloat compute_period(stk::StkFloat phi, stk::StkFloat estimate) {
  }


} // class SolaStream


int main() {
  SolaStream sola_stream;
  mu::LinsegStream phi_stream;
  mu::LinSegStream tau_stream;
  mu::LoopStream loop_stream;
  mu::RtPlayer player;

  sola_stream.setPhiSource(&phi_stream).setTauSource(&tau_stream);
  sola_stream.setSampleSource(getSoundFile(SOUND_DIR "A4" ".wav"));
  sola_stream.setPhiPrevs();
  sola_stream.setPhiNexts();

  loop_stream.setSource(&sola_stream);

  player.setSource(&loop_stream);
  
  player.start();
  fprintf(stderr, "Type [return] to quit:");  getchar();
  player.stop();

  return 0;
}

                        

