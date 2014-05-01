SOURCES := \
	crop_stream.cpp \
	delay_stream.cpp \
	file_read_stream.cpp \
	identity_stream.cpp \
	loop_stream.cpp \
	map_stream.cpp \
	mix_stream.cpp \
	mu.cpp \
	nrt_player.cpp \
	player.cpp \
	rt_player.cpp \
	stream.cpp \
	stream_extent.cpp

OBJECT_DIR := ../obj
INCLUDE_DIR := ../include

OBJECTS := $(patsubst %.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

CC      = g++
DEFS    = 
CFLAGS  = -O3 -Wall
IFLAGS  = -I$(INCLUDE_DIR) -I../tarballs/stk-4.4.4/include

$(OBJECT_DIR)/%.o : %.cpp $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/mu.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $(<) -o $@

all: $(OBJECTS)

clean:
	rm -f $(OBJECT_DIR)/*.o

$(OBJECTS): | $(OBJECT_DIR)

$(OBJECT_DIR):
	mkdir $(OBJECT_DIR)
