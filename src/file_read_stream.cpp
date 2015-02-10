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

#include "file_read_stream.h"
#include "mu_utils.h"
#include "player.h"

namespace mu {

  stk::FileRead *FileReadStream::lookup(std::string file_name) {
    SoundLibrary::const_iterator iterator;

    if ((iterator = library_.find(file_name)) == library_.end()) {
        // printf("reading '%s' from disk\n", file_name.c_str());
        stk::FileRead *file_read = new stk::FileRead();
        file_read->open(file_name);
        library_[file_name] = file_read;
        return file_read;
      } else {
        // printf("fetching '%s' from cache\n", file_name.c_str());
        return iterator->second;
    }
  }

  FileReadStream::FileReadStream()
    : file_read_(NULL),
      file_name_("") {
    tmp_buffer_.resize(Player::default_frame_size(), 
                       Player::default_channel_count());
  }

  FileReadStream::~FileReadStream() {
    // printf("~FileReadStream()\n");
  }

  FileReadStream *FileReadStream::clone() {
    FileReadStream *c = new FileReadStream();
    c->set_file_name(file_name());
    return c;
  }

  // Return true if there is an open sound file and its parameters (data type,
  // sample rate, number of channels) match the frames object.  TODO: This would
  // be a good candidate for a "before_playing()" callback.  TODO: return a
  // string describing the mismatch (if any)
  bool FileReadStream::verify_format(MuBuffer *buffer) {
    // nit-pick: no need for "else" (but takes less vertical space)
    if (!file_read_->isOpen()) {
      return false;
    } else if (file_read_->channels() != buffer->channels()) {
      return false;
    } else if (file_read_->fileRate() != buffer->dataRate()) {
      return false;
    } else {
      return true;
    }
  }

  bool FileReadStream::render(MuTick buffer_start, MuBuffer *buffer) {

    // MuUtils::assert_empty(buffer);

#if 0
    // check for format mismatch
    if (!verify_format(buffer)) {
      // printf("!"); fflush(stdout);
      return false;
    }
#endif

    MuTick n_src_frames = file_read_->fileSize();
    int n_src_channels = file_read_->channels();
    MuTick file_end = n_src_frames;

    MuTick n_dst_frames = buffer->frames();
    int n_dst_channels = buffer->channels();
    MuTick buffer_end = n_dst_frames + buffer_start;

    if ((buffer_end <= 0) || (buffer_start >= file_end)) {
      // Nothing to render
      return false;
    }

    if ((buffer_start >= 0) && (buffer_end <= file_end)) {
      // Render complete buffer
      if (n_src_channels == n_dst_channels) {
        // common case can be rendered directly
        file_read_->read(*buffer, buffer_start);
        return true;

      } else {
        return copy_samples(buffer_start, buffer_end, buffer_start, buffer);
      }

    } else {
      // Render partial buffer
      MuTick lo = std::max((MuTick)0, buffer_start);
      MuTick hi = std::min(buffer_end, file_end);

      return copy_samples(lo, hi, buffer_start, buffer);
    }
  }

  // Fetch frames (lo...hi] from sound file, copy them into buffer with
  // appropriate offset.  Also handles mixing or spreading to match number of
  // channels in buffer->channels();
  bool FileReadStream::copy_samples(MuTick lo, MuTick hi, MuTick buffer_start, MuBuffer *buffer) {
    MuTick n_frames = hi - lo;

    if (n_frames <= 0) {
      // zero length file?
      return false;
    }

    int n_src_channels = file_read_->channels();
    int n_dst_channels = buffer->channels();

    // prepare to mix or spread samples
    tmp_buffer_.resize(n_frames, n_src_channels);
    MuUtils::zero_buffer(&tmp_buffer_);
    file_read_->read(tmp_buffer_, lo);
    
    if ((n_src_channels == 1) && (n_dst_channels == 2)) {
      // spread from one src to two dst
      // TODO: optimize indexing
      for (MuTick tick = lo; tick < hi; ++tick) {
        MuFloat sample = tmp_buffer_(tick - lo, 0);
        (* buffer)(tick - buffer_start, 0) = sample;
        (* buffer)(tick - buffer_start, 1) = sample;
      }
      
    } else if ((n_src_channels == 2) && (n_dst_channels == 1)) {
      // mix two src to one dst
      for (MuTick tick = lo; tick < hi; ++tick) {
        MuFloat sample = (tmp_buffer_(tick - lo, 0) + tmp_buffer_(tick - lo, 1)) * 0.5;
        (* buffer)(tick - buffer_start, 0) = sample;
      }
      
    } else {
      // combine all src and spread to all dst
      for (MuTick tick = lo; tick < hi; ++tick) {
        MuFloat sample = 0.0;
        for (int channel = n_src_channels-1; channel >= 0; --channel) {
          sample += tmp_buffer_(tick - lo, channel);
        }
        sample = sample / (double)n_src_channels;
        for (int channel = n_dst_channels-1; channel >= 0; --channel) {
          (* buffer)(tick - buffer_start, channel) = sample;
        }
      }
    }
    return true;
    
  }
  
  void FileReadStream::inspect_aux(int level, std::stringstream *ss) {
    MuStream::inspect_aux(level, ss);
    inspect_indent(level, ss);
    *ss << "file_name() = " << file_name() << std::endl;
  }

  // globally available...

  FileReadStream::SoundLibrary FileReadStream::library_;


}                               // namespace mu

