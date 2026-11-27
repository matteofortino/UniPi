#include "handlers.h"
#include "../coordinator.h"
#include "../common/common.h"
#include "../io/io.h"

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int find_storage_slot(void) {
    for (int i = 0; i < MAX_STORAGES; i++) {
        if (!storage[i].valid)
            return i;
    }
    return -1;
}

int get_storage_slot_by_fd(int fd) {
    for (int i = 0; i < MAX_STORAGES; i++) {
        if (storage[i].fd == fd)
            return i;
    }
    return -1;
}

void clear_storage(int fd) {
    pthread_mutex_lock(&state_mutex);
    int slot = get_storage_slot_by_fd(fd);
    if (slot < 0) {
        pthread_mutex_unlock(&state_mutex);
        return;
    }

    uint32_t sid = storage[slot].id;

    storage[slot].port = 0;
    storage[slot].valid = 0;
    storage[slot].id = 0;
    storage[slot].fd = -1;

    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].valid && files[i].storage_id == sid) {
            files[i].valid = 0;
        }
    }

    printf("Storage %d disconnected\n", storage_id);

    pthread_mutex_unlock(&state_mutex);
}

int find_file_slot(void) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].valid)
            return i;
    }
    return -1;
}

int handle_register(int fd, uint16_t port, uint32_t *out_id) {
    pthread_mutex_lock(&state_mutex);

    int slot = find_storage_slot();
    if (slot < 0) {
        pthread_mutex_unlock(&state_mutex);
        const char *err = protocol_error_str(PROTO_ERR_IO);
        send_all(fd, err, strlen(err));
        return -1;
    }
    uint32_t id = storage_id++;
    storage[slot].valid = 1;
    storage[slot].id = id;
    storage[slot].port = port;
    storage[slot].fd = fd;
    pthread_mutex_unlock(&state_mutex);
    char resp[MAX_LINE_LENGTH];
    int len = snprintf(resp, sizeof(resp), "REGISTERED %u\n", id);
    if (len < 0 || (size_t)len >= sizeof(resp) || send_all(fd, resp, (size_t)len) != IO_OK) {
        clear_storage(fd);
        return -1;
    }
    *out_id = id;
    return 0;
}

int handle_stored(int fd, uint32_t sid, size_t size, char *remote_name) {
    pthread_mutex_lock(&state_mutex);
    if (sid == 0 || remote_name == NULL) {
        pthread_mutex_unlock(&state_mutex);
        return -1;
    }

    int slot = find_file_slot();
    if (slot < 0) {
        pthread_mutex_unlock(&state_mutex);
        const char *err = protocol_error_str(PROTO_ERR_IO);
        send_all(fd, err, strlen(err));
        return -1;
    }

    files[slot].valid = 1;
    files[slot].size = size;
    files[slot].storage_id = sid;
    strcpy(files[slot].remote_name, remote_name);

    pthread_mutex_unlock(&state_mutex);

    char resp[] = "ACK\n";
    int len = strlen(resp);
    if (len < 0 || (size_t)len >= sizeof(resp) || send_all(fd, resp, (size_t)len) != IO_OK) {
        clear_storage(fd);
        return -1;
    }
    return 0;
}
