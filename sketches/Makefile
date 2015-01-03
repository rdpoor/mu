MU_ROOT ?= ../usr

SOURCES := $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp,%,$(SOURCES))

CC       = g++
ifdef MU_DEBUG
CFLAGS   = -O0 -g -Wall
else
CFLAGS   = -O3 -Wall
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

