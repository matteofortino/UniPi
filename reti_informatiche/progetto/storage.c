#include "storage.h"
#include "io/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int directory_exists(const char *path) {
    struct stat sb;
    if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode))
        return 1; // directory exists

    return 0; // doen not exists or it is not a directory
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: %s <coordinator_port> <storage_port> <remote_directory>\n", argv[0]);
        return EXIT_FAILURE;
    }

    uint16_t coordinator_port = atoi(argv[1]);
    if (coordinator_port < 1024) { // invalid coordinator port
        printf("<coordinator_port> needs to be >= 1024\n");
        return EXIT_FAILURE;
    }

    uint16_t storage_port = atoi(argv[2]);
    if (storage_port < 1024) { // invalid storage port
        printf("<storage_port> needs to be >= 1024\n");
        return EXIT_FAILURE;
    }

    if (storage_port == coordinator_port) { // same ports
        printf("<coordinator_port> needs to be different from <storage_port>\n");
        return EXIT_FAILURE;
    }

    if (!directory_exists(argv[3])) { // directory does not exists
        printf("Directory %s not found\n", argv[3]);
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

    struct sockaddr_in storage_addr;
    storage_addr.sin_family = AF_INET;
    storage_addr.sin_port = htons(storage_port);
    inet_pton(AF_INET, LOCALHOST, &storage_addr.sin_addr);

    if (connect(coordinator_fd, (struct sockaddr *)&coordinator_addr,
                (socklen_t)sizeof(coordinator_addr)) < 0) {
        perror("connect");
        return EXIT_FAILURE;
    }

    char msg[256];
    snprintf(msg, sizeof(msg), "REGISTER %d\n", storage_port);
    send_all(coordinator_fd, msg, strlen(msg));
    recv_control_msg(coordinator_fd, msg);
    printf("Coordinator: %s", msg);
}
