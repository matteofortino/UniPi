#ifndef COORDINATOR_H
#define COORDINATOR_H

#include "common/common.h"

#include <pthread.h>

#define MAX_STORAGES 100
#define MAX_FILES 1024
typedef struct {
    int8_t valid;
    uint32_t id;
    uint16_t port;
    int fd;
} Storage;

typedef struct {
    int8_t valid;
    size_t size;
    char remote_name[MAX_NAME_LENGTH + 1];
    uint32_t storage_id;
} File;

extern pthread_mutex_t state_mutex;
extern Storage storage[MAX_STORAGES];
extern uint32_t storage_id;
extern File files[MAX_FILES];

void initialize_storage(Storage *storage);
void *worker(void *arg);
#endif
