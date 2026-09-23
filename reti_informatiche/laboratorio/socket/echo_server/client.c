#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return 1;
    }

    char *host = argv[1];
    int port = atoi(argv[2]);
    int sd, ret;
    struct sockaddr_in sv_addr;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return 1;
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &sv_addr.sin_addr);

    ret = connect(sd, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if (ret < 0) {
        perror("connect");
        close(sd);
        return 1;
    }

    printf("Connected to server %s:%d\n", host, port);

    while (1) {
        char message[20];
        char buffer[20];

        printf("Enter a message: ");
        if (!fgets(message, sizeof(message), stdin))
            break;
        message[strcspn(message, "\n")] = '\0';

        send(sd, message, strlen(message), 0);

        int bytes = recv(sd, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
            printf("Server closed the connection.\n");
            break;
        }
        buffer[bytes] = '\0';
        printf("Server replied: %s\n", buffer);

        if (strcmp(message, "Bye") == 0)
            break;
    }

    close(sd);
    printf("Connection closed.\n");
    return 0;
}
