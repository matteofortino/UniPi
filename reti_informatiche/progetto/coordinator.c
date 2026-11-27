#include "coordinator.h"
#include "common/common.h"
#include "io/io.h"
#include "handlers/handlers.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t state_mutex;
Storage storage[MAX_STORAGES];
uint32_t storage_id = 1;
File files[MAX_FILES];

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t coordinator_port = 0;
    if (parse_port(argv[1], &coordinator_port) != PROTO_OK || coordinator_port < 1024) {
        printf("<coordinator_port> must be a valid port (1024-65535)\n");
        return EXIT_FAILURE;
    }

    int coordinator_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (coordinator_fd < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in coordinator_addr;
    coordinator_addr.sin_family = AF_INET;
    coordinator_addr.sin_port = htons(coordinator_port);
    inet_pton(AF_INET, LOCALHOST, &coordinator_addr.sin_addr);

    if (bind(coordinator_fd, (struct sockaddr *)&coordinator_addr,
             (socklen_t)sizeof(coordinator_addr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }

    if (listen(coordinator_fd, 100) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }
    pthread_mutex_init(&state_mutex, NULL);
    initialize_storage(storage);

    while (1) {
        int fd = accept(coordinator_fd, NULL, NULL);
        if (fd < 0)
            continue;
        int *pfd = malloc(sizeof(int));
        if (pfd == NULL) {
            close(fd);
            continue;
        }
        *pfd = fd;
        pthread_t tid;
        if (pthread_create(&tid, NULL, worker, pfd) != 0) {
            close(fd);
            free(pfd);
            continue;
        }
        pthread_detach(tid);
    }
    close(coordinator_fd);
    pthread_exit(NULL);
} // END MAIN

void initialize_storage(Storage *storage) {
    for (int i = 0; i < MAX_STORAGES; i++) {
        storage[i].valid = 0;
        storage[i].id = 0;
        storage[i].port = 0;
        storage[i].fd = -1;
    }
}

void *worker(void *arg) {
    uint32_t my_id = 0;
    int storage_fd = *(int *)arg;
    free(arg);

    printf("Worker creato %d\n", storage_fd);

    while (1) {
        char line[MAX_LINE_LENGTH];
        int ret = recv_line(storage_fd, line, MAX_LINE_LENGTH);

        if (ret != IO_OK) {
            if (ret == IO_LINE_TOO_LONG || ret == IO_BAD_CHAR)
                send_error(storage_fd, PROTO_ERR_BAD_REQUEST);
            clear_storage(storage_fd);
            break;
        }

        ProtocolError err;

        char *fields[MAX_FIELDS] = {NULL};
        int count;
        err = split_fields(line, fields, &count);
        if (err != PROTO_OK) {
            const char *msg = protocol_error_str(err);
            send_all(storage_fd, msg, strlen(msg));
            clear_storage(storage_fd);
            break;
        }

        Request request;
        err = parse_request(fields, count, &request);
        if (err != PROTO_OK) {
            send_error(storage_fd, err);
            clear_storage(storage_fd);
            break;
        }

        if (request.command == CMD_REGISTER) {
            if (my_id != 0) {
                send_error(storage_fd, PROTO_ERR_BAD_REQUEST);
                clear_storage(storage_fd);
                break;
            }
            handle_register(storage_fd, request.storage_port, &my_id);
        } else if (request.command == CMD_STORED) {
            if (my_id == 0) {
                send_error(storage_fd, PROTO_ERR_BAD_REQUEST);
                clear_storage(storage_fd);
                break;
            }
            handle_stored(storage_fd, my_id, request.size, request.remote_name);
        }
    }

    close(storage_fd);
    pthread_exit(NULL);
}
