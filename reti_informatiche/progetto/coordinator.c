#include "coordinator.h"
#include "io/io.h"

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

Storage storage[100];

void *handle_request(void *arg);

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t coordinator_port = atoi(argv[1]);

    if (coordinator_port < 1024) {
        printf("<port> needs to be >= 1024\n");
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

    while (1) {
        int fd = accept(coordinator_fd, NULL, NULL);
        pthread_t tid;
        pthread_create(&tid, NULL, handle_request, &fd);
    }
}
int id = 1;
void *handle_request(void *arg) {
    int storage_fd = *(int *)arg;
    char msg[256];
    recv_control_msg(storage_fd, msg);

    printf("storage %d: %s", storage_fd, msg);
    snprintf(msg, sizeof(msg), "REGISTERED %d\n", id);
    send_all(storage_fd, msg, strlen(msg));
    storage[id].id = id;
    pthread_exit(NULL);
}
