CC = gcc
CFLAGS = -Wall -Wextra -g -pthread

# Programmi da generare
TARGETS = utente lavagna

all: $(TARGETS)

utente: utente.o
	$(CC) $(CFLAGS) -o utente utente.o

lavagna: lavagna.o
	$(CC) $(CFLAGS) -o lavagna lavagna.o

# Regole di compilazione
utente.o: utente.c common.h
	$(CC) $(CFLAGS) -c utente.c

lavagna.o: lavagna.c common.h
	$(CC) $(CFLAGS) -c lavagna.c

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean
