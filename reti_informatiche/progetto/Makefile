CC = gcc
CFLAGS = -Wall -Wextra -g

# Source directories
SRC_UTIL = utility

# Object files
OBJ_UTIL = $(SRC_UTIL)/comandi.o $(SRC_UTIL)/lista_utenti.o
OBJ_LAVAGNA = lavagna.o $(OBJ_UTIL)
OBJ_UTENTE  = utente.o  $(OBJ_UTIL)

# Executables (nella root)
TARGET_LAVAGNA = lavagna
TARGET_UTENTE  = utente

all: $(TARGET_LAVAGNA) $(TARGET_UTENTE)

# Build executables
$(TARGET_LAVAGNA): $(OBJ_LAVAGNA)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_UTENTE): $(OBJ_UTENTE)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation rules
lavagna.o: lavagna.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

utente.o: utente.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_UTIL)/comandi.o: $(SRC_UTIL)/comandi.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_UTIL)/lista_utenti.o: $(SRC_UTIL)/lista_utenti.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f *.o $(SRC_UTIL)/*.o $(TARGET_LAVAGNA) $(TARGET_UTENTE)

clear: clean

.PHONY: all clean clear
