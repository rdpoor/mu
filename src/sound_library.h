/* 
   ================================================================
   Copyright (C) 2014-2015 Robert D. Poor
  
   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:
   
   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.  

   ================================================================
*/

// File: sound_library.h
// Shared resource to map sound file names into stk::FileRead objects.

#ifndef MU_SOUND_LIBRARY_H
#define MU_SOUND_LIBRARY_H

#include <FileRead.h>
#include <unordered_map>

namespace mu {

  class SoundLibrary {
  public:

    typedef std::unordered_map<std::string, stk::FileRead *> Repository;

    SoundLibrary() {
    }

    ~SoundLibrary() {
      for (Repository::const_iterator it = map_.begin(); it != map_.end(); ++it ) {
        delete it->second;
      }
    }

    stk::FileRead *lookup(std::string file_name) {
      Repository::const_iterator got = map_.find(file_name);
      if (got == map_.end()) {
        // printf("reading '%s' from disk\n", file_name.c_str());
        stk::FileRead *file_read = new stk::FileRead();
        file_read->open(file_name);
        map_[file_name] = file_read;
        return file_read;
      } else {
        // printf("fetching '%s' from cache\n", file_name.c_str());
        return got->second;
      }
    }

  protected:
    Repository map_;

  };                            // class SoundLibrary

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
