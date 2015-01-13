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