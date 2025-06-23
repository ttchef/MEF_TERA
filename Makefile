
CC := gcc 
GCCFLAGS := -O2 -Wall -g

CFILES := $(wildcard *.c)
OFILES := $(CFILES:.c=.o)
OUTPUT := main
BIN := bin

all: $(OUTPUT)

%.o: %.c 
	$(CC) $(GCCFLAGS) -c $< -o $(BIN)/$@

$(OUTPUT): $(OFILES)
	$(CC) -o $(OUTPUT) $(BIN)/$(OFILES)


