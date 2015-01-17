=== RenderStreams to write / test:

* add_rs.h [done]
* constant_rs.h [done]
* crop_rs.h [done]
* delay_rs.h [done]
* dirac_rs.h [done]
* file_read_rs.h [done]
* identity_rs.h [done]
* line_segment_rs [done]
* loop_rs.h [done]
* multi_source_rs.h [implicitly]
* multiply_rs.h [done]
* random_select_rs.h
* sine_rs.h
* single_source_rs.h [implicitly]
* stk_effect_rs.h [done]
* xfade_rs.h 

=== TODO

* crop_rs should write zeros outside of its start and end
  range. [done, but see Design Ponder below.]
* bool RenderStream::render() return false when frames left untouched.
  (See Design Ponder below)
* Should linseg_rs work over an open interval (includes starting
  point, excludes ending point) or closed (includes both)?  Now it
  works over an open interval, which means if you ramp from 10.0 to
  1.0, it will never emit the 1.0.  That might be surprising to some. [done]
* Reinstate introspection (from previous versions) to allow printing
  and eventual GUI.

=== Design Decision

Calls to RenderStream::render() should leave frames untouched outside
of the range [start_frame, end_frame).  In addition, RenderStreams of
finite extent (currently CropRS, FileReadRS, LinsegRS) should leave
frames untouched outside of their extent.

As an optimization, RenderStream::render() will return false if
nothing in frames was touched, signalling to the caller that nothing
needs to be done.

=== But what about Empty Sum and Empty Products?

For AddRS and MultiplyRS, what should the effect be if there are no
inputs contributing to the output?  Some options (using MultiplyRS as
an example):

* Always emit 1.0 when there are zero inputs contributing and always
  return true, i.e. the stream has infinite extent.

* Always emit 1.0 when there are zero inputs contributing and return
  false, i.e. the stream has extent determined by its inputs.  (That's
  weird because it violates the contract render()'s return value.)

* Don't modify the frames buffer if there are no inputs contributing
  and return false (that's the current behavior).  If you want
  something different, you can attach a ConstantRS(1.0) as an input
  which will give it infinite extent.

=== XFadeRS

Originally I was thinking of writing a RS that takes one input and
fades it in at the beginning and out at the end.

Then I thought that it should take two inputs and crossfade from one
to the other at a specific time.

Then I thought it should take N inputs, each with a start time (?)
and crossfade from one to the next.

For now I think I'll just wait until I need it.

=== LoopRS

I wonder if the semantics of LoopRS should be changed to create an
infinite series of its input without cropping.  One implication is
that the input to LoopRS must have finite extent, and that the extent
be available to LoopRS.

In addition, the arguments to render (base_tick, start_tick, end_tick)
are almost always start_tick == base_tick and end_tick = base_tick +
frame_count.  It would be simpler (and less error prone) to reduce
the args to render(base_tick).

And if I do that, I'm really close to the original LoopSP framework
where every SP defined its start and end times.  Maybe that was a good
idea after all.


