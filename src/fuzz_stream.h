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

/*
 * FuzzStream implements a fuzzbox.  fuzz_factor sets the fuzziness: 0.0 is no
 * fuzziness, 1.0 is very fuzzy.  mix sets the blend betwee original signal and
 * effect: 0.0 is 100% original signal, 1.0 is 100% effect.
 */

#if 0
// discussion
// The current implementation f(x) = x^(1.0 - fuzz_factor), but it doesn't sound
// particularly good.  So perhaps a cubic polynomial of the form:
//
// [01] f(x) = ax^3 + bx^2 + cx + d
// [02] f'(x) = 3ax^2 + 2bx + c
//
// Given m, and also given:
//
// [03] f(0) = 0
// [04] f(1.0) = 1.0
// [05] f'(0) = m
// [06] f'(1.0) = m
//
// ...solve for a, b, c, d:
//
// solve for d:
// [07] d = 0 (subst [03] into [01])
//
// solve for c:
// [09] c = m (subst [05] into [02])
//
// solve for b:
// [11] 1 = a + b + m (subst [04] into [01])
// [12] a = 1 - b - m (solve [11] for a)
// [13] m = 3a + 2b + m (subst [06] into [02])
// [14] m = 3(1 - b - m) + 2b + m (subst [12] into [13])
// [15] m = 3 - 3b - 3m + 2b + m (simplify...)
// [16] 3b - 2b = 3 - 3m + m - m (simplify...)
// [17] b = 3 - 3m
// 
// solve for a
// [18] a = 1 - (3 - 3m) - m (subst [17] into [12])
// [19] a = 1 - 3 + 3m - m (simplify)
// [20] a = 2m - 2
//
// ergo 
// a = 2m - 2
// b = 3 - 3m
// c = m
// d = 0
//
// Test:
// f(0) = 0
// f(1) = a + b + c + d =? 1
// f(1) = 2m - 2 + 3 - 3m + m + 0
//      = 1 (check)
// f'(0) = c = m (check)
// f'(1) = 3a + 2b + c
//       = 3(2m - 2) + 2(3 - 3m) + m
//       = 6m - 6 + 6 - 6m + m
//       = m (check)

// ================================================================
// same but with different conditions:
//
// [01] f(x) = ax^3 + bx^2 + cx + d
// [02] f'(x) = 3ax^2 + 2bx + c
//
// Given m, and also given:
//
// [03] f(0) = 0
// [04] f(1.0) = 1.0
// [05] f'(0) = m
// [06] f'(1.0) = 1/m
//
// ...solve for a, b, c, d:
//
// solve for d:
// [07] d = 0 (subst [03] into [01])
//
// solve for c:
// [09] c = m (subst [05] into [02])
//
// solve for b:
// [11] 1 = a + b + m (subst [04] into [01])
// [12] a = 1 - b - m (solve [11] for a)
// [13] 1/m = 3a + 2b + m (subst [06] into [02])
// [14] 1/m = 3(1 - b - m) + 2b + m (subst [12] into [13])
// [15] 1/m = 3 - 3b - 3m + 2b + m (simplify...)
// [16] 3b - 2b = 3 - 3m + m - 1/m (simplify...)
// [17] b = 3 - 2m - 1/m
// 
// solve for a
// [18] a = 1 - (3 - 2m - 1/m) - m (subst [17] into [12])
// [19] a = 1 - 3 + 2m + 1/m - m (simplify)
// [20] a = -2 + m + 1/m
//
// ergo 
// a = -2 + m + 1/m
// b = 3 - 2m - 1/m
// c = m
// d = 0
//
// Test:
// f(0) = 0
// f(1) = a + b + c + d =? 1
// f(1) = - 2 + 3 - m + m + 0
//      = 1 (check)
// f'(0) = c = m (check)
// f'(1) = 3a + 2b + c
//       = 3(-2) + 2(3 - m) + m
//       = -6 + 6 - 2m + m
//       = -m (check)

#endif

#ifndef MU_FUZZ_STREAM_H
#define MU_FUZZ_STREAM_H

#include "mu_types.h"
#include "single_source_stream.h"

namespace mu {

  typedef MuFloat (FuzzStreamCallback)(MuFloat sample, MuTick tick);
  
  class FuzzStream : public SingleSourceStream {
  public:

    FuzzStream( void );
    virtual ~FuzzStream( void );
    virtual FuzzStream *clone( void );

    // 0.0 = original signal, 1.0 = fuzz only
    // TODO: make mix part of an EffectsStream virtual class
    MuFloat mix() { return mix_; }
    void set_mix(MuFloat mix) { mix_ = mix; }

    // 1.0 = clean, 0.1 = really fuzzy
    MuFloat fuzz_factor() { return fuzz_factor_; }
    void set_fuzz_factor(MuFloat fuzz_factor) { fuzz_factor_ = fuzz_factor; }

    bool render(MuTick buffer_start, MuBuffer *buffer);

    std::string get_class_name() { return "FuzzStream"; }

  protected:
    void inspect_aux(int level, std::stringstream *ss);
    MuFloat mix_;
    MuFloat fuzz_factor_;

  };                            // class FuzzStream

}                               // namespace mu

#endif

// Local Variables:
// mode: c++
// End:
