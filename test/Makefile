MU_ROOT := ../usr

SOURCES := $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp,%,$(SOURCES))

CC       = g++
ifdef $(MU_DEBUG)
CFLAGS   = -O0 -g -Wall
else
CFLAGS   = -O3 -Wall
endif

LFLAGS   = -L$(MU_ROOT)/lib
DEFS     = 
INCLUDES = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi -lm -lgsl -lstk -lmu

.PHONY:	clean

all :	$(TARGETS)

clean :
	rm -f $(TARGETS)
	rm -rf *.dSYM

$(TARGETS) : %:%.cpp assert.o
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(LIBRARY)

run-tests : $(TARGETS)
	for x in $(TARGETS); do echo ======== $$x; ./$$x; done

unit-tests :	$(UNIT_TEST_TARGETS)

