MU_ROOT ?= /Users/r/Projects/Mu/usr

RELEASE = 0.0.1

# .cpp files to include in /usr/lib/libmu.a, corresponding .h files
# are copied to /usr/include directory
SOURCES := \
	constant_stream.cpp \
	crop_stream.cpp \
	delay_stream.cpp \
	dirac_stream.cpp \
	file_read_stream.cpp \
	identity_stream.cpp \
	loop_stream.cpp \
	mu_stream.cpp \
	mu_utils.cpp \
	multi_source_stream.cpp \
	player.cpp \
	player_rt.cpp \
	product_stream.cpp \
	resample_stream.cpp \
	sine_stream.cpp \
	single_source_stream.cpp \
	sum_stream.cpp \
	transport.cpp

# .h files that lack a corresponding .cpp file but should be
# copied to /usr/include directory
INCLUDES := \
	mu_types.h

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

%.o : %.cpp %.h $(INCLUDES)
	$(CC) $(CFLAGS) $(IFLAGS) -c $(<) -o $@

all: $(OBJECTS)

install: $(STATICLIB) $(LIB_DIR) $(HEADER_DIR) $(INCLUDES) # $(SHAREDLIB) -- sharedlib doesn't work yet
	mv $(STATICLIB) $(MU_ROOT)/lib
	cp -p $(INCLUDES) $(HEADERS) $(HEADER_DIR)

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

