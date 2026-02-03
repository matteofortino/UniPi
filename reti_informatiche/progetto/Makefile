CC = gcc
CFLAGS = -Wall -g -O2
TARGETS = lavagna utente

all: $(TARGETS)

lavagna: lavagna.c common.h
	$(CC) $(CFLAGS) -o lavagna lavagna.c

utente: utente.c common.h
	$(CC) $(CFLAGS) -o utente utente.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
