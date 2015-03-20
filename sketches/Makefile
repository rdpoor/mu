MU_ROOT ?= ../usr
# BOOST_ROOT ?= $(MU_ROOT)/boost_1_57_0

SOURCES := $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp,%,$(SOURCES))

CC       = g++
ifdef MU_DEBUG
CFLAGS   = -O0 -g -Wall -std=c++11
else
CFLAGS   = -O3 -Wall -std=c++11
endif

DEFS     = 
INCLUDES = 
LIBRARY  = 

.PHONY:	clean

all :	$(TARGETS)

clean :
	rm -f $(TARGETS)
	rm -rf *.dSYM

$(TARGETS) : %:%.cpp
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFS) $(INCLUDES) -o $(@) $(<) $(LIBRARY)

zzz :
	( export BAR=gleep echo $(BAR) ; printenv ; echo $(BAR) )

sketch_hiredis :	sketch_hiredis.c
	$(CC) -x c $(CFLAGS) -L $(MU_ROOT)/lib $(LFLAGS) $(DEFS) -I $(MU_ROOT)/include/hiredis -o $(@) $(<) -lhiredis

sketch_hiredis02 :	sketch_hiredis02.c
	$(CC) -x c $(CFLAGS) -L $(MU_ROOT)/lib $(LFLAGS) $(DEFS) -I $(MU_ROOT)/include/hiredis -o $(@) $(<) -lhiredis

sketch_hiredis03 :	sketch_hiredis03.c
	$(CC) -x c $(CFLAGS) -L $(MU_ROOT)/lib $(LFLAGS) $(DEFS) -I $(MU_ROOT)/include/hiredis -I $(MU_ROOT)/include -o $(@) $(<) -lhiredis -ljansson

# t12 :	t12.cpp
#	$(CC) $(CFLAGS) -L $(MU_ROOT)/lib $(LFLAGS) $(DEFS) -I $(MU_ROOT)/include -o $(@) $(<) -ljansson
