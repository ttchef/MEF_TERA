
CC := gcc 
GCCFLAGS := -O2 -Wall -g
LDFLAGS := -lglfw -lGL

CFILES := $(wildcard *.c)
OFILES := $(CFILES:.c=.o)
OUTPUT := main

all: $(OUTPUT)

%.o: %.c 
	$(CC) $(GCCFLAGS) -c $< -o $@

$(OUTPUT): $(OFILES)
	$(CC) -o $(OUTPUT) $(OFILES) $(LDFLAGS)


