MU_ROOT := /Users/r/Projects/Mu/usr

SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))

CC       = g++
ifdef MU_DEBUG
  CFLAGS   = -O0 -g -Wall
else
  CFLAGS   = -O3 -Wall
endif

LFLAGS   = -L$(MU_ROOT)/lib
DEFS     = 
INCLUDES = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi -lm -lgsl -lstk -lmu

.PHONY:	clean

all :	tnvm2

clean :
	rm -f tnvm2 $(OBJECTS)
	rm -rf *.dSYM

%.o : %.cpp tnvm2.h
	$(CC) $(CFLAGS) $(IFLAGS) $(INCLUDES) -c $(<) -o $@

tnvm2 : $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(OBJECTS) $(LIBRARY)
