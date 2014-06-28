MU_ROOT = ../usr
RELEASE = 0.0.1

SOURCES := \
	add_sp.cpp \
	channelizer_sp.cpp \
	constant_sp.cpp \
	crop_sp.cpp \
	delay_sp.cpp \
	fade_sp.cpp \
	file_read_sp.cpp \
	file_write_sp.cpp \
	identity_sp.cpp \
	linseg_sp.cpp \
	loop_sp.cpp \
	map_sp.cpp \
	mu.cpp \
	multi_source_sp.cpp \
	multiply_sp.cpp \
	nrt_player.cpp \
	null_sp.cpp \
	pitch_utilities.cpp \
	player.cpp \
	probability_sp.cpp \
	psi_sp.cpp \
	psi_waveform.cpp \
	random_select_sp.cpp \
	resample_sp.cpp \
	reverb_sp.cpp \
	rt_player.cpp \
	sample_processor.cpp \
	sequence_sp.cpp \
	sine_sp.cpp \
	single_source_sp.cpp \
	splice_sp.cpp

OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
HEADERS := $(patsubst %.cpp,%.h,$(SOURCES))

AR = ar -rsc
RM = /bin/rm
LN = /bin/ln

STATICLIB = libmu.a
SHAREDLIB = libmu.dylib

# destinations for make install
LIB_DIR := $(MU_ROOT)/lib
HEADER_DIR := $(MU_ROOT)/include/mu

CC      = g++
DEFS    = 
ifdef MU_DEBUG
  CFLAGS = -g -O0 -Wall  
else
  CFLAGS  = -O3 -Wall
endif

IFLAGS = -I$(MU_ROOT)/include/mu -I$(MU_ROOT)/include/stk -I$(MU_ROOT)/include

%.o : %.cpp %.h mu.h
	$(CC) $(CFLAGS) $(IFLAGS) -c $(<) -o $@

all: $(OBJECTS)

install: $(STATICLIB) $(LIB_DIR) $(HEADER_DIR) # $(SHAREDLIB) -- sharedlib doesn't work yet
	mv $(STATICLIB) $(MU_ROOT)/lib
	cp -p $(HEADERS) $(HEADER_DIR)

clean:
	rm -f $(OBJECTS)

$(STATICLIB) : all
	$(AR) -cr $(STATICLIB) $(OBJECTS)

# not ready for prime time
# https://developer.apple.com/library/mac/documentation/DeveloperTools/Conceptual/DynamicLibraries/000-Introduction/Introduction.html
# see https://developer.apple.com/library/mac/documentation/DeveloperTools/Conceptual/DynamicLibraries/100-Articles/CreatingDynamicLibraries.html
$(SHAREDLIB) : all
	$(RM) -f $(@) $(SHAREDLIB).$(MAJOR) $(SHAREDLIB)
	$(CC) $(LDFLAGS) -fPIC -dynamiclib -o libmu.$(RELEASE).dylib *.o $(LIBS)
	$(LN) -s libmu.$(RELEASE).dylib $(SHAREDLIB)

$(HEADER_DIR):
	mkdir -p $(HEADER_DIR)

$(LIB_DIR): 
	mkdir -p $(LIB_DIR)

# psi_file_generator : %:%.cpp $(INCLUDE_DIR)/%.h $(INCLUDE_DIR)/mu.h
# 	$(CC) $(CFLAGS) $(LFLAGS) $(IFLAGS) -o $(@) $(<) $(STKLIB) $(LIBRARY)

