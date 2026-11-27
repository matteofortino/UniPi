CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2
PTHREAD = -pthread
COMMON_SRC = io/io.c common/common.c
COMMON_HEADERS = io/io.h common/common.h

.PHONY: all clean
all: coordinator storage # client


coordinator: coordinator.c coordinator.h handlers/handlers.c handlers/handlers.h $(COMMON_SRC) $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $(PTHREAD) coordinator.c handlers/handlers.c $(COMMON_SRC) -o $@

storage: storage.c storage.h $(COMMON_SRC) $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $(PTHREAD) storage.c $(COMMON_SRC) -o $@

# client: client.c $(COMMON) io/io.h protocol.h
# 	$(CC) $(CFLAGS) $(PTHREAD) client.c $(COMMON) -o $@

clean:
	rm -f coordinator storage client
