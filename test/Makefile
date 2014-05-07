INTEGRATION_TEST_TARGETS := $(patsubst %.cpp,%,$(wildcard mune*.cpp))

UNIT_TEST_TARGETS := $(patsubst %.cpp,%,$(wildcard ut_*.cpp))

SKETCH_TARGETS := $(patsubst %.cpp,%,$(wildcard test_*.cpp))

ALL_TARGETS := $(INTEGRATION_TEST_TARGETS) $(UNIT_TEST_TARGETS) $(SKETCH_TARGETS)

CC       = g++
CFLAGS   = -O3 -Wall # or perhaps -O0 -g -Wall
DEFS     = -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__
INCLUDES = -I../include -I../tarballs/stk-4.4.4/include
MULIB   := ../obj/*.o
STKLIB  := ../tarballs/stk-4.4.4/src/Release/*.o
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi

.PHONY:	clean

all :	integration-tests unit-tests sketches

clean :
	rm -f $(ALL_TARGETS)

integration-tests :	$(INTEGRATION_TEST_TARGETS)

run-integration-tests : integration-tests
	for x in $(INTEGRATION_TEST_TARGETS); do echo ======== $$x; ./$$x; done

$(INTEGRATION_TEST_TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

unit-tests :	$(UNIT_TEST_TARGETS)

run-unit-tests : unit-tests
	for x in $(UNIT_TEST_TARGETS); do echo ======== $$x; ./$$x; done

$(UNIT_TEST_TARGETS) : %:%.cpp assert.o
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

sketches :	$(SKETCH_TARGETS)

$(SKETCH_TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) -o $(@) $(<)
