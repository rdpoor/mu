#include "splay_stream.h"
#include <strings.h>
#include <math.h>

namespace mu {

  static const double kTwoPi = M_PI * 2.0;

  SplayStream::SplayStream()
    : period_ ( 100 ),
      n_periods_ ( 100 ),
      sample_source_ ( NULL ), 
      theta_source_ ( NULL ),
      phi_source_ ( NULL ) {
    sample_buffer_.resize(0, 2);
    theta_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
    phi_buffer_.resize(stk::RT_BUFFER_SIZE, 2);
  }

  SplayStream::~SplayStream() {
    TRACE("SplayStream::~SplayStream()\n");
  }

  void SplayStream::inspectAux(std::stringstream& ss, int level) {
    inspectIndent(ss, level); ss << "SampleSource" << std::endl;
    ss << sample_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "ThetaSource" << std::endl;
    ss << theta_source_->inspect(level+1);
    inspectIndent(ss, level); ss << "PhiSource" << std::endl;
    ss << phi_source_->inspect(level+1);
  }

  SplayStream& SplayStream::step(stk::StkFrames& buffer,
                                 Tick tick,
                                 Player& player) {
    
    // fprintf(stderr,"\ntick: theta, phi, i11, i21, i12, i22, alpha1, alpha2, beta\n");

    zeroBuffer(buffer);
    if ((sample_source_ == NULL) || (theta_source_ == NULL) || (phi_source_ == NULL)) {
      return *this;
    }

    // Get theta, phi and the sample waveform
    theta_buffer_.resize(buffer.frames(), buffer.channels());
    theta_source_->step(theta_buffer_, tick, player);
    phi_buffer_.resize(buffer.frames(), buffer.channels());
    phi_source_->step(phi_buffer_, tick, player);

    // TODO: If sample_count_ is too large, we could always 
    // take a pass through theta and phi to find the first and
    // last samples required from sample_source and fetch
    // accordingly.
    //
    if (sample_buffer_.frames() != getNFrames()) {
      sample_buffer_.resize(getNFrames(), buffer.channels());
      sample_source_->step(sample_buffer_, 0, player);
    }

    for (int frame=0; frame<theta_buffer_.frames(); frame++) {
      // Note that the user inputs specify theta ranging from
      // 0 to 2Pi (for one period).  We change units so theta
      // ranges from 0 to period_.
      stk::StkFloat theta = fmod(theta_buffer_(frame, 0) * period_ / kTwoPi, period_);
      stk::StkFloat phi = phi_buffer_(frame, 0);

      Tick i11, i21, i12, i22;
      stk::StkFloat q11, q21, q12, q22, r1, r2, alpha1, alpha2, beta, p;

      // get indeces into sample_buffer of the four samples
      // surrounding [theta, phi]
      i11 = floor(theta + floor(phi) * period_);
      i21 = i11 + 1;
      i22 = ceil(theta + ceil(phi) * period_);
      i12 = i22 - 1;
      // TODO: clamp properly!
      if ((i11 < 0) || (i12 < 0) || 
          (i21 >= sample_buffer_.frames()) || (i22 >= sample_buffer_.frames())) {
        break;
      }

      // r1 = interpolate between q11 and q21
      alpha1 = fmod(theta - i11, period_);
      if (alpha1 < 0.0) alpha1 += period_;
      if (alpha1 > 1.0) alpha1 -= period_;
      q11 = sample_buffer_(i11, 0);
      q21 = sample_buffer_(i21, 0);
      r1 = q11 + (q21 - q11) * alpha1;

      // r2 = interpolate between q12 and q22
      alpha2 = fmod(theta - i12, period_);
      if (alpha2 < 0.0) alpha2 += period_;
      if (alpha2 > 1.0) alpha2 -= period_;
      q12 = sample_buffer_(i12, 0);
      q22 = sample_buffer_(i22, 0);
      r2 = q12 + (q22 - q12) * alpha2;

      // p = interpolate between r1 and r2
      beta = fmod(phi, 1) - theta/period_;
      p = r1 + (r2 - r1) * beta;

#if 0
      fprintf(stderr,
              "%ld: %f, %f, %ld, %ld, %ld, %ld, %f, %f, %f\n",
              tick + frame,
              theta,
              phi,
              i11, i21, i12, i22, alpha1, alpha2, beta);
#endif
              
      for (int channel = 0; channel < buffer.channels(); channel++) {
        buffer(frame, channel) = p;
      }
    }
    return *this;
  }


}


