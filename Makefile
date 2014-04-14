PROGRAMS    = mune
SOURCE_PATH = ./src
OBJECT_PATH = ./obj
INCLUDE     = ./tarballs/stk-4.4.4/include

CC       = g++
DEFS     =
CFLAGS   = -O3 -Wall -I$(INCLUDE)
LIBRARY  = ./tarballs/stk-4.4.4/src/Release/RtAudio.o
LIBRARY  += -lpthread -framework CoreAudio -framework CoreFoundation

%.o : $(SOURCE_PATH)/%.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -c $(<) -o $(OBJECT_PATH)/$@

all : $(PROGRAMS)

mune: $(SOURCE_PATH)/mune.cpp
	$(CC) $(LDFLAGS) $(CFLAGS) $(DEFS) -o mune $(<) $(LIBRARY)
