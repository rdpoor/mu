SOURCES := \
	mune01.cpp \
	mune02.cpp \
	mune03.cpp \
	mune04.cpp \
	mune05.cpp \
	mune06.cpp \
	mune07.cpp \
	mune08.cpp \
	mune09.cpp \
	mune10.cpp \
	mune11.cpp \
	mune12.cpp \
	mune13.cpp \
	mune14.cpp \
	mune15.cpp \
	mune16.cpp \
	mune17.cpp \
	mune18.cpp \
	test_clamp.cpp \
	test_double_accuracy.cpp \
	test_map.cpp \
	test_pthread.cpp \
	test_subclass.cpp \
	test_vector.cpp \
	ut_add_stream.cpp \
	ut_constant_stream.cpp \
	ut_crop_stream.cpp \
	ut_delay_stream.cpp \
	ut_identity_stream.cpp \
	ut_linseg_stream.cpp \
	ut_multiply_stream.cpp \
	ut_stream.cpp \
	ut_tick_indefinite.cpp

MULIB   := ../obj/*.o
STKLIB  := ../tarballs/stk-4.4.4/src/Release/*.o

CC       = g++
DEFS     = -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__
CFLAGS   = -O3 -Wall 
# CFLAGS   = -O0 -g -Wall 
INCLUDES = -I../include -I../tarballs/stk-4.4.4/include
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi


# this rule is not recognized.  
# $(patsubst %.cpp,%,$(SOURCES)) : $@.cpp 
# 	echo at=$@ pct=$% hat=$^ star=$*

all: $(patsubst %.cpp,%,$(SOURCES))

clean:
	rm -f $(patsubst %.cpp,%,$(SOURCES))

test : all
	./ut_crop_stream
	./ut_delay_stream
	./ut_identity_stream
	./ut_mix_stream
	./ut_stream

assert.o : assert.c
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -c $(<) -o $(@)

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

mune08 : mune08.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune09 : mune09.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune10 : mune10.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune11 : mune11.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune12 : mune12.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune13 : mune13.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune14 : mune14.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune15 : mune15.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune16 : mune16.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune17 : mune17.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

mune18 : mune18.cpp # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

test_double_accuracy: test_double_accuracy.cpp
	$(CC) $(CFLAGS) -o $(@) $(<)

test_clamp: test_clamp.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

test_map: test_map.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

test_pthread: test_pthread.cpp
	$(CC) $(CFLAGS) -o $(@) $(<)

test_vector: test_vector.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -o $(@) $(<) $(MULIB) $(STKLIB) $(LIBRARY)

ut_add_stream : ut_add_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_constant_stream : ut_constant_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_crop_stream : ut_crop_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_delay_stream : ut_delay_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_identity_stream : ut_identity_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_linseg_stream : ut_linseg_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_multiply_stream : ut_multiply_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_stream : ut_stream.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

ut_tick_indefinite : ut_tick_indefinite.cpp assert.o # | $(MULIB) $(STKLIB)
	$(CC) $(CFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) assert.o $(MULIB) $(STKLIB) $(LIBRARY)

