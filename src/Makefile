# Figure out how to define SOURCES without manually naming each file
SOURCES := mu.cpp node.cpp test_node.cpp transport.cpp
OBJECT_DIR := ../obj
INCLUDE_DIR := ../include

OBJECTS := $(patsubst %.cpp,$(OBJECT_DIR)/%.o,$(SOURCES))

CC      = g++
DEFS    = 
CFLAGS  = -O3 -Wall
IFLAGS  = -I$(INCLUDE_DIR) -I../tarballs/stk-4.4.4/include

$(OBJECT_DIR)/%.o : %.cpp $(INCLUDE_DIR)/mu.h # $(INCLUDE_DIR)/$*.h 
	$(CC) $(CFLAGS) $(IFLAGS) -c $(<) -o $@

all: $(OBJECTS)

clean:
	rm -f $(OBJECT_DIR)/*.o

$(OBJECTS): | $(OBJECT_DIR)

$(OBJECT_DIR):
	mkdir $(OBJECT_DIR)
