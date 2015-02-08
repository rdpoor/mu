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
#include "psi_waveform.h"

namespace mu {

  PsiWaveform *PsiWaveform::lookup( std::string file_name ) {
    PsiLibrary::iterator iterator;

    if ((iterator = library_.find(file_name)) == library_.end()) {
      PsiWaveform * psi_waveform = new PsiWaveform();
      psi_waveform->initialize_from_file(file_name);
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
    period_buffer_.resize(0, 1);
  }

  PsiWaveform::~PsiWaveform() {
  }

  void PsiWaveform::initialize_from_file(std::string file_name) {
    read_psi_file(file_name);
    file_name_ = file_name;
  }

  // A .psi file is a text representation of a waveform.  It has the format:
  //
  // Version_Number
  // Frame Count
  // s[0], d[0], p[0]
  // s[1], d[1], p[1]
  // ...
  // s[frame_count-1], d[frame_count-1], p[frame_count-1]
  //
  // where:
  // s[t] sample at frame t
  // d[t] derivative of s[t]
  // p[t] period of waveform at frame t
  //
  void PsiWaveform::read_psi_file(std::string file_name) {
    int n_items_read, n_lines_read, n_frames;
    double s, p;
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
    period_buffer_.resize(n_frames, 1);

    n_lines_read = 0;
    for (int i=0; i<n_frames; i++) {
      if (getline(&line, &linecap, ifd) == EOF) {
        fprintf(stderr, "Premature end of file.  Read %d out of %d.\n", n_lines_read, n_frames);
        goto err;
      }
      n_items_read = sscanf(line, "%lf, %lf\n", &s, &p);
      if (n_items_read != 2) {
        fprintf(stderr, "at line %d: unrecognized format: %s\n", n_lines_read, line);
        goto err;
      }
      sample_buffer_[i] = s;
      period_buffer_[i] = p;
      n_lines_read += 1;
    }

    fprintf(stderr, "Read %d frames from %s\n", n_frames, file_name.c_str());
  err:
    fclose(ifd);
  }

  // globally available...

  PsiWaveform::PsiLibrary PsiWaveform::library_;

} // namespace mu

