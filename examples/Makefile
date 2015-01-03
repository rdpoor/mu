MU_ROOT := ../usr

SOURCES := $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp,%,$(SOURCES))

CC       = g++
ifdef MU_DEBUG
CFLAGS   = -O0 -g -Wall
else
CFLAGS   = -O3 -Wall
endif

LFLAGS   = -L$(MU_ROOT)/lib
DEFS     = 
INCLUDES = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk -I$(MU_ROOT)/include/hiredis -I$(MU_ROOT)/include
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi -lm -lgsl -lstk -lhiredis -lmu

.PHONY:	clean

all :	$(TARGETS)

clean :
	rm -f $(TARGETS)
	rm -rf *.dSYM

$(TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(LIBRARY)

