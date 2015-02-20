== Instruments 

* drums
** Kick
* Snare Proxy ("pshhhhhht")
** Tom Toms (with lots of reverb)
** shaker 1
** shaker 2
* percussion / accents
** cres/decres percussion
** martian breedles
** room tone
* pitched
** alto pluck
** strums
** fuzz
** bass
** vowcals
** "something organ like"

== Production
* stereo spatialization
* reverb(s)
* sweepable filters
* dynamics
* splicers

== File organization
* One file per instrument class: inst_xxx.cpp
* One *stream() per instrument
* One file per instrument notes: xxx_notes.cpp
* One file for instrument effects/production: xxx_fx.cpp
* One file for effects automation: xxx_fxmoves.cpp

== Music ideas

Opening notes: F# B E C# unprocessed on uke, with a little "thump" and "swish"
(also unprocessed). The thump and swish morph into the rhythm part, the pitched
notes get octave shifted, stretched (in time) and compressed (in dynamics).
Then mutiple comb filters are applied to create F#, B, E, C# chords.

Therefore need:
* pitch shifting (PsiStream would be good)
* log/exp ramp (like LinSegStream)
* dynamic compression (need EnvelopeFollowerStream and DynamicCompressionStream)
* comb filter (CombFilterStream)

== Code/Composition Overview

Each instrument has a .h and a .cpp file.  The .h file defines one
public method:

    // inst_clacker.h
    #ifndef INST_CLACKER_H
    #define INST_CLACKER_H
    #include "tnvm.h"
    mu::ProcessingStream *make_clacker_ps();
    #endif

whose contract is to return a ProcessingStream that renders that one
instrument.

At present, I believe that each instrument controls its own gain,
reverb, binuaral localization, etc.  At the end, all instruments 
will mix together for final gain and equalization.

Classes that are generally useful for the composition, but aren't
general enough to be included in the mu:: library will be here.
They are distinguished by not having an inst_* prefix on their
file (and class) names.
