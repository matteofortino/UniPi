CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_UTIL = utility
BIN = bin

# Object files
OBJ_UTIL = $(SRC_UTIL)/comandi.o $(SRC_UTIL)/lista_utenti.o
OBJ_LAVAGNA = lavagna.o $(SRC_UTIL)/comandi.o $(SRC_UTIL)/lista_utenti.o
OBJ_UTENTE  = utente.o  $(SRC_UTIL)/comandi.o $(SRC_UTIL)/lista_utenti.o

# Executables
TARGET_LAVAGNA = $(BIN)/lavagna
TARGET_UTENTE  = $(BIN)/utente

all: dirs $(TARGET_LAVAGNA) $(TARGET_UTENTE)

dirs:
	mkdir -p $(BIN)

# Build executables
$(TARGET_LAVAGNA): $(OBJ_LAVAGNA)
	$(CC) $(CFLAGS) -o $@ $^

$(TARGET_UTENTE): $(OBJ_UTENTE)
	$(CC) $(CFLAGS) -o $@ $^

# Compile modules in utility/
$(SRC_UTIL)/comandi.o: $(SRC_UTIL)/comandi.c all.h $(SRC_UTIL)/lista_utenti.o
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_UTIL)/lista_utenti.o: $(SRC_UTIL)/lista_utenti.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile main programs
lavagna.o: lavagna.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

utente.o: utente.c all.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(SRC_UTIL)/*.o

.PHONY: all clean dirs
