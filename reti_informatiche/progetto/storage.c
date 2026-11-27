#include "storage.h"
#include "common/common.h"
#include "io/io.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

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

    uint16_t coordinator_port = 0;
    if (parse_port(argv[1], &coordinator_port) != PROTO_OK || coordinator_port < 1024) {
        printf("<coordinator_port> must be a valid port (1024-65535)\n");
        return EXIT_FAILURE;
    }

    uint16_t storage_port = 0;
    if (parse_port(argv[2], &storage_port) != PROTO_OK || storage_port < 1024) {
        printf("<storage_port> must be a valid port (1024-65535)\n");
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

    printf("Storage connesso\n");
    char in_msg[MAX_LINE_LENGTH];
    snprintf(in_msg, sizeof(in_msg), "REGISTER %d\n", storage_port);
    send_all(coordinator_fd, in_msg, strlen(in_msg));

    printf("REGISTER SENT\n");
    char out_msg[MAX_LINE_LENGTH];
    if (recv_line(coordinator_fd, out_msg, sizeof(out_msg)) != IO_OK) {
        close(coordinator_fd);
        return EXIT_FAILURE;
    }
    printf("%s\n", out_msg);
    snprintf(in_msg, sizeof(in_msg), "STORED ciao cane di dio\n");
    send_all(coordinator_fd, in_msg, strlen(in_msg));

    printf("STORED SENT\n");
    if (recv_line(coordinator_fd, out_msg, sizeof(out_msg)) != IO_OK) {
        close(coordinator_fd);
        return EXIT_FAILURE;
    }
    printf("%s\n", out_msg);
}
