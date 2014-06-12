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

#ifndef MU_PSI_WAVEFORM_H
#define MU_PSI_WAVEFORM_H

#include "mu.h"
#include <unordered_map>

namespace mu {

  class PsiWaveform {
  public:

    typedef std::unordered_map<std::string, PsiWaveform *> PsiLibrary;

    // fetch or create an initialized PsiWaveform object
    static PsiWaveform *lookup( std::string file_name );

    PsiWaveform( void );
    ~PsiWaveform( void );

    Tick getFrameCount() { return sample_buffer_.frames(); }

    std::string getFileName( void ) { return file_name_; }

    // Fetch the t'th sample from sample_buffer_ using linear
    // interpolation.  Returns 0.0 if t is out of range.
    stk::StkFloat getFSample( double t ) {
      if ((t < 0) || (t >= sample_buffer_.frames())) {
        return 0.0;
      } else {
        long int i = (long int)t;
        double alpha = t - i;
        return sample_buffer_[i] + alpha * dsample_buffer_[i];
      }
    }

    // Get the period of the waveform in the vicinity of tau
    stk::StkFloat getPeriod(stk::StkFloat tau);

  protected:
    static PsiLibrary library_;

    std::string file_name_;
    stk::StkFrames sample_buffer_;  // samples
    stk::StkFrames dsample_buffer_; // forward difference of sample_buffer_ 
    stk::StkFrames period_buffer_;  // period of sample_buffer_

  private:

    void initializeFromFile( std::string file_name );

    void readPsiFile(std::string file_name);

  }; // class PsiWaveform

} // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
