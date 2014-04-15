# http://martinvseticka.eu/temp/make/presentation.html looks like a good tutorial.

SOURCE_PATH = ./src
OBJECT_PATH = ./obj
BINARY_PATH = ./bin
PROGRAMS    = $(BINARY_PATH)/mune01 $(BINARY_PATH)/mune02
INCLUDE     = ./tarballs/stk-4.4.4/include

CC       = g++
DEFS     = -DHAVE_GETTIMEOFDAY -D__MACOSX_CORE__ -D__LITTLE_ENDIAN__
CFLAGS   = -O3 -Wall -I$(INCLUDE)
LIBRARY  = -lpthread -framework CoreAudio -framework CoreFoundation -framework CoreMidi

%.o : $(SOURCE_PATH)/%.cpp
	$(CC) $(CFLAGS) $(DEFS) -c $(<) -o $(OBJECT_PATH)/$@

all : $(PROGRAMS)

STK_OBJECT_PATH = ./tarballs/stk-4.4.4/src/Release
STK_OBJECT_FILES = $(STK_OBJECT_PATH)/*.o

mune01: $(SOURCE_PATH)/mune01.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -o $(BINARY_PATH)/$(@) $(<) $(STK_OBJECT_FILES) $(LIBRARY)

mune02: $(SOURCE_PATH)/mune02.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -o $(BINARY_PATH)/$(@) $(<) $(STK_OBJECT_FILES) $(LIBRARY)

clean:
	rm -f $(PROGRAMS)
