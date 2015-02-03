/*
  ================================================================
  Copyright (C) 2014-2015 Robert D. Poor
  
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
#include "random_select_stream.h"
#include "mu_utils.h"
#include <random>

namespace mu {

  RandomSelectStream::RandomSelectStream( void ) :
    needs_recompute_(true),
    current_stream_(NULL),
    discrete_distribution_(NULL) {
  }

  RandomSelectStream::~RandomSelectStream( void ) {
    for (int i=stream_descriptors_.size()-1; i>=0; --i) {
      delete stream_descriptors_.at(i);
    }
  }

  RandomSelectStream *RandomSelectStream::clone() {
    RandomSelectStream *c = new RandomSelectStream();
    for (int i=stream_descriptors_.size()-1; i>=0; --i) {
      RandomSelectStream::StreamDescriptor *sd = stream_descriptors_.at(i);
      c->add_source(MuUtils::clone_stream(sd->source()), sd->probability());
    }
    return c;
  }

  void RandomSelectStream::add_source(MuStream *source, MuFloat probability) {
    if ((source == NULL) || probability == 0.0) return;
    RandomSelectStream::StreamDescriptor *sd = 
      new RandomSelectStream::StreamDescriptor(source, probability);
    stream_descriptors_.push_back(sd);
    needs_recompute_ = true;
  }

  bool RandomSelectStream::render(MuTick buffer_start, MuBuffer *buffer) {
    if (stream_descriptors_.size() == 0) return false;
    if (needs_recompute_) recompute();
    if (buffer_start == 0) select();

    return current_stream_->render(buffer_start, buffer);
  }

  void RandomSelectStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "sources()" << std::endl;
    for (int i=stream_descriptors_.size()-1; i>=0; --i) {
      RandomSelectStream::StreamDescriptor *sd = stream_descriptors_.at(i);
      inspect_indent(level+1, ss);
      *ss << "probability: " << sd->probability();
      sd->source()->inspect_aux(level+1, ss);
    }
  }

  // Extract list of weights from StreamDescriptors and initialize a new
  // discrete_distribution from those weights.
  //
  void RandomSelectStream::recompute( void ) {
    std::vector<double> weights;
    std::vector<RandomSelectStream::StreamDescriptor *>::iterator it;
    for (it = stream_descriptors_.begin(); it != stream_descriptors_.end(); ++it) {
      weights.push_back((*it)->probability());
    }
    if (discrete_distribution_) delete discrete_distribution_;
    discrete_distribution_ = 
      new std::discrete_distribution<int>(weights.begin(), weights.end());
    needs_recompute_ = false;
  }

  void RandomSelectStream::select( void ) {
    int i = (*discrete_distribution_)(random_generator_);
    current_stream_ = stream_descriptors_.at(i)->source();
  }


}                               // namespace mu
