MU_ROOT = ../usr
RELEASE = 0.0.1

EXECUTABLES := \
	psi_file_generator

SOURCES := $(patsubst %,%.cpp,$(EXECUTABLES))

AR = ar -rsc
RM = /bin/rm
LN = /bin/ln

CC      = g++
ifdef MU_DEBUG
  CFLAGS = -g -O0 -Wall
else
  CFLAGS = -O3 -Wall
endif

IFLAGS = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk -I$(MU_ROOT)/include

LFLAGS = -L$(MU_ROOT)/lib

LIBRARIES = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi -lfftw3 -lm -lgsl -lstk -lmu

# executable depends on src.cpp, src.h, and all of the headers in the distro
% : %.cpp %.h $(MU_ROOT)/include/mu/mu.h
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) $(<) -o $(@) $(LIBRARIES)

all: $(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES)

