# Compile Mu examples.  Filenames must be in the form "mu_*.cpp" to be
# recognized.

MU_ROOT ?= /Users/r/Projects/Mu/usr

SOURCES := $(wildcard mu_*.cpp)
TARGETS := $(patsubst %.cpp,%,$(SOURCES)) mu_19

RUBY     = /usr/bin/ruby
CC       = g++
ifdef MU_DEBUG
CFLAGS   = -O0 -g -Wall -std=c++11
else
CFLAGS   = -O3 -Wall -std=c++11
endif

LFLAGS   = -L$(MU_ROOT)/lib
DEFS     = 
INCLUDES = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk -I$(MU_ROOT)/include
LIBRARY  = -lpthread \
	-framework CoreAudio \
	-framework CoreFoundation \
	-framework CoreMidi \
	-lm -lgsl -lstk -lmu -lcurses

.PHONY:	clean

all :	$(TARGETS)

clean :
	rm -f $(TARGETS) mu_19.cpp
	rm -rf *.dSYM

$(TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(LIBRARY)

# Experiment: use Ruby to generate a c++ file which is then compiled normally.
mu_19.cpp : mu_19.rb
	$(RUBY) mu_19.rb mu_19.cpp


