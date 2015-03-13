CC=gcc
CFLAGS=-Wall -g -pedantic -std=c99 -DVERSION=\"$(GIT_VERSION)\"
LDFLAGS=-lm -lreadline
SOURCES=parsing.c read.c evaluate.c print.c types.c mpc/mpc.c
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=slur
GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

all: $(SOURCES) $(EXECUTABLE)
	    
$(EXECUTABLE): $(OBJECTS) 
	    $(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	    $(CC) $< -o $@

.PHONY: clean

clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
