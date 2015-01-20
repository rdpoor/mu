# Unit Test Driver
# To make all tests:
#   $ make tests
# To run all tests:
#   $ ./tests
# To run a subset of tests:
#   $ ./tests --gtest_filter=LoopRS*
#
# Test files must be named *_test.cpp in order to be recognized.

MU_ROOT ?= /Users/r/Projects/Mu/usr

SOURCES := $(wildcard *_test.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))
OBJECTS := $(patsubst %.cpp, %.o, $(SOURCES))

CPPFLAGS += -isystem $(MU_ROOT)/include
CXXFLAGS += -g -Wall -Wextra -pthread
IFLAGS   += -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk 
LFLAGS   += -L$(MU_ROOT)/lib

install : tests

all : tests

objects : $(OBJECTS)

clean :
	rm -rf tests $(TARGETS) *.dSYM $(OBJECTS)

# $< is name of prerequisite and $@ is the name of the target
# Note that gmock_main.a needs to be linked in order to define _main();

%.o : %.cpp $(MU_ROOT)/lib
	$(CXX) $(IFLAGS) $(CPPFLAGS) $(CXXFLAGS) -c $(<) -o $(@)

tests : $(OBJECTS)
	$(CXX) $(LFLAGS) $(OBJECTS) \
		-lpthread -lgmock -lmu -lstk $(MU_ROOT)/lib/gmock_main.a -o $@
