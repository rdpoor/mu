SOURCES := mune01.cpp mune02.cpp mune03.cpp mune04.cpp mune05.cpp mune06.cpp mune07 \
           test_double_accuracy.cpp test_clamp.cpp test_subclass.cpp \
	   test_pthread.cpp

MULIB   := ../obj/*.o
STKLIB  := ../tarballs/stk-4.4.4/src/Release/*.o

CC       = g++
DEFS     = -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__
CFLAGS   = -O3 -Wall 
INCLUDES = -I../include -I../tarballs/stk-4.4.4/include
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi


# this rule is not recognized.  
# $(patsubst %.cpp,%,$(SOURCES)) : $@.cpp 
# 	echo at=$@ pct=$% hat=$^ star=$*

all: $(patsubst %.cpp,%,$(SOURCES))

clean:
	rm -f $(patsubst %.cpp,%,$(SOURCES))

mune01 : mune01.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune02 : mune02.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune03 : mune03.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune04 : mune04.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(STKLIB) $(LIBRARY)

mune05 : mune05.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune06 : mune06.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune07 : mune07.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

test_double_accuracy: test_double_accuracy.cpp
	$(CC) $(CFLAGS) -o $(@) $(<)

test_clamp: test_clamp.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

test_pthread: test_pthread.cpp
	$(CC) $(CFLAGS) -o $(@) $(<)
