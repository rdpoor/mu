# Unit Test Driver
# To run all tests:
#   make run-tests
# To run one test:
#   make hello_test && ./hello_test 
#
# Test files must be named *_test.cpp in order to be recognized.

MU_ROOT ?= /Users/r/Projects/Mu/usr

SOURCES := $(wildcard *_test.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

CPPFLAGS += -isystem $(MU_ROOT)/include
CXXFLAGS += -g -Wall -Wextra -pthread
IFLAGS   += -I$(MU_ROOT)/include/mu
LFLAGS   += -L$(MU_ROOT)/lib

all : $(TARGETS)

clean :
	rm -rf $(TARGETS) *.dSYM

run-tests : $(TARGETS)
	for x in $(TARGETS); do echo ====== $$x; ./$$x; done

# $< is name of prerequisite and $@ is the name of the target
# Note that gmock_main.a needs to be linked in order to define _main();
$(TARGETS) : %:%.cpp $(MU_ROOT)/lib
	$(CXX) $(IFLAGS) $(LFLAGS) $(CPPFLAGS) $(CXXFLAGS) $< -lpthread -lgmock -lmu $(MU_ROOT)/lib/gmock_main.a -o $@

