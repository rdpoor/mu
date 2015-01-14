=== RenderStreams to test:

* add_rs.h [done]
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

=== RenderStreams to write:

* FadeIn/FadeOut(start_tick, end_tick, ramp_rate) (What Would SOX Do?)

=== TODO

* crop_rs should write zeros outside of its start and end
  range. [done, but see Design Ponder below.]
* bool RenderStream::render() return false when frames left untouched.
  (See Design Ponder below)
* Should linseg_rs work over an open interval (includes starting
  point, excludes ending point) or closed (includes both)?  Now it
  works over an open interval, which means if you ramp from 10.0 to
  1.0, it will never emit the 1.0.  That might be surprising to some.
* Reinstate introspection (from previous versions) to allow printing
  and eventual GUI.

=== Design Ponder

What should a RenderStream do when asked to render something outside
of its range?  For example, imagine a one second sound file asked to
render something at time=100.0.  Should it always zero the frame
buffer before returning it?  Imagine if there are thousands of sound
files -- that's a lot of zeroing.  

I could rewrite the contract of render() to return false when the
frames buffer was untouched.  Then the caller of render() would just
skip whatever it was about to do with the frames buffer.

I think that works, and it's much more natural than each RenderStream
object maintaining startTick() and endTick() methods.

So far, the only RenderStream classes with a finite extent are CropRS
and FileReadRS.  I need to write tests for the latter before changing
the contract of RenderStream.render().  LineSegmentRS will also have
finite extent.

