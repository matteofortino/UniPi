#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define LOCALHOST "127.0.0.1"

typedef struct {
    size_t size;
    char *name;
} File;

typedef struct {
    uint16_t id;
    char *storage_directory;
    File *files;

} Storage;

#endif
