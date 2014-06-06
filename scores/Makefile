SCORE_TARGETS := $(patsubst %.cpp,%,$(wildcard *.cpp))

ALL_TARGETS := $(SCORE_TARGETS)

CC       = g++
# TODO: switch flags via command line arg
# CFLAGS   = -O3 -Wall
CFLAGS   = -O0 -g -Wall
DEFS     = -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__
INCLUDES = -I../include -I../tarballs/stk-4.4.4/include
MULIB   := ../obj/*.o
# TODO: switch libraries via command line arg
STKLIB  := ../tarballs/stk-4.4.4/src/Release/*.o
# STKLIB  := ../tarballs/stk-4.4.4/src/Debug/*.o
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi

.PHONY:	clean

all :	scores

clean :
	rm -f $(ALL_TARGETS)

scores :	$(SCORE_TARGETS)

$(SCORE_TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

