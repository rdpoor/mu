SOURCES := \
	add_stream.cpp \
	constant_stream.cpp \
	crop_stream.cpp \
	delay_stream.cpp \
	file_read_stream.cpp \
	identity_stream.cpp \
	linseg_stream.cpp \
	loop_stream.cpp \
	map_stream.cpp \
	mu.cpp \
	multi_source_stream.cpp \
	multiply_stream.cpp \
	nrt_player.cpp \
	player.cpp \
	probability_stream.cpp \
	random_select_stream.cpp \
	rt_player.cpp \
	sequence_stream.cpp \
	single_source_stream.cpp \
	stream.cpp \
	tik.cpp

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
