CC = gcc 
CFLAGS = -Wall -Wextra 


all: coordinator storage io
	@ mkdir -p storage_dir

coordinator: coordinator.c io/io.c
	$(CC) $(CFLAGS) $^ -o $@

storage: storage.c io/io.c
	$(CC) $(CFLAGS) $^ -o $@

io: io/io.c 
	$(CC) $(CFLAGS) $^ -o $@
