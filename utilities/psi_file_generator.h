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

#ifndef MU_PSI_FILE_GENERATOR_H
#define MU_PSI_FILE_GENERATOR_H

#include <mu.h>

namespace mu {

  class PsiFileGenerator {
  public:

    PsiFileGenerator( void ) {}
    ~PsiFileGenerator( void ) {}

    // Read in and analyze .wav file
    void analyzeFile(std::string wav_file_name, 
                     std::string psi_file_name,
                     double estimated_period);

    // write data to psi file (only after analyzeFile() has been
    // called)
    void writePsiData();

    // compute median period (only after analyzeFile() has been
    // called)
    double medianPeriod();

    // Not public, but needs to be visible as a callback to the GSL
    // minimization library.
    double min_correlation_aux(double period, double tau);

  protected:
    std::string wav_file_name_;
    std::string psi_file_name_;
    stk::StkFrames sample_buffer_;  // samples
    stk::StkFrames dsample_buffer_; // forward difference of sample_buffer_ 
    stk::StkFrames period_buffer_;  // period of sample_buffer_
    double estimated_period_;

  private:
    void zeroBuffer(stk::StkFrames &b);
    void computePeriods();
    double computePeriod(void *handle, double tau, bool chatty);

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

  };                            // class PsiFileGenerator
  
}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
