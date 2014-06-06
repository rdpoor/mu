SOURCES := \
	add_stream.cpp \
	constant_stream.cpp \
	crop_stream.cpp \
	delay_stream.cpp \
	fade_stream.cpp \
	file_read_stream.cpp \
	file_write_stream.cpp \
	identity_stream.cpp \
	linseg_stream.cpp \
	loop_stream.cpp \
	map_stream.cpp \
	mu.cpp \
	multi_source_stream.cpp \
	multiply_stream.cpp \
	nrt_player.cpp \
	null_stream.cpp \
	player.cpp \
	probability_stream.cpp \
	random_select_stream.cpp \
	resample_stream.cpp \
	reverb_stream.cpp \
	rt_player.cpp \
	sequence_stream.cpp \
	sine_stream.cpp \
	single_source_stream.cpp \
	splay_stream.cpp \
	splice_stream.cpp \
	stream.cpp \
	tik.cpp

OBJECT_DIR := ../obj
INCLUDE_DIR := ../include

OBJECTS := $(patsubst %.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

CC      = g++
DEFS    = 
# TODO: Switch flags via command line args
# CFLAGS  = -O3 -Wall
CFLAGS = -g -O0 -Wall
IFLAGS  = -I$(INCLUDE_DIR) -I../tarballs/stk-4.4.4/include

$(OBJECT_DIR)/%.o : %.cpp $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/mu.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $(<) -o $@

all: $(OBJECTS)

clean:
	rm -f $(OBJECT_DIR)/*.o

$(OBJECTS): | $(OBJECT_DIR)

$(OBJECT_DIR):
	mkdir $(OBJECT_DIR)
