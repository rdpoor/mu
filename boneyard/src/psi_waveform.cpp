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
#include "psi_waveform.h"

namespace mu {

  PsiWaveform *PsiWaveform::lookup( std::string file_name ) {
    PsiLibrary::iterator iterator;

    if ((iterator = library_.find(file_name)) == library_.end()) {
      PsiWaveform * psi_waveform = new PsiWaveform();
      psi_waveform->initializeFromFile(file_name);
      library_[file_name] = psi_waveform;
      return psi_waveform;
    } else {
      // TODO: use the iterator's value since it's already found the
      // element we want...
      return library_[file_name];
    }
  }

  PsiWaveform::PsiWaveform( void ) {
    sample_buffer_.resize(0, 1);
    dsample_buffer_.resize(0, 1);
    period_buffer_.resize(0, 1);
  }

  PsiWaveform::~PsiWaveform() {
  }

  void PsiWaveform::initializeFromFile(std::string file_name) {
    readPsiFile(file_name);
    file_name_ = file_name;
  }

  // Get the period around t=tau in the waveform.
  stk::StkFloat PsiWaveform::getPeriod(stk::StkFloat tau) {
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

  void PsiWaveform::readPsiFile(std::string file_name) {
    int n_items_read, n_lines_read, n_frames;
    double x, y, z;
    char *line = NULL;
    size_t linecap = 0;

    FILE* ifd = fopen(file_name.c_str(), "r");
    if (NULL == ifd) {
      // TODO: need to actually handle the error somehow...
      fprintf(stderr, "Failed to open %s for reading.\n", file_name.c_str());
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

    fprintf(stderr, "Read %d frames from %s\n", n_frames, file_name.c_str());
  err:
    fclose(ifd);
  }

  // globally available...

  PsiWaveform::PsiLibrary PsiWaveform::library_;

} // namespace mu

