#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_SIZE 1024

// Struttura per passare il socket al thread
typedef struct {
    int socket;
    struct sockaddr_in addr;
} client_info_t;

// Funzione che gestisce un singolo client
void *handle_client(void *arg) {
    client_info_t *client = (client_info_t *)arg;
    int new_sd = client->socket;

    printf("New client thread started.\n");

    while (1) {
        char buffer[MAX_SIZE];
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(new_sd, buffer, sizeof(buffer), MSG_WAITALL);
        if (bytes <= 0) {
            printf("Client disconnected.\n");
            close(new_sd);
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';
        printf("Message received: %s\n", buffer);

        send(new_sd, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "Bye") == 0) {
            printf("Client said Bye — closing connection.\n");
            close(new_sd);
            break;
        }
    }

    free(client);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    int sd, new_sd, ret;
    struct sockaddr_in my_addr, cl_addr;
    socklen_t len;

    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return 1;
    }

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (ret < 0) {
        perror("bind");
        close(sd);
        return 1;
    }

    ret = listen(sd, 10);
    if (ret < 0) {
        perror("listen");
        close(sd);
        return 1;
    }

    printf("Server listening on port %d\n", port);

    while (1) {
        len = sizeof(cl_addr);
        new_sd = accept(sd, (struct sockaddr*)&cl_addr, &len);
        if (new_sd < 0) {
            perror("accept");
            continue;
        }

        printf("New client connected!\n");

        // Alloco struttura client e creo thread
        client_info_t *client = malloc(sizeof(client_info_t));
        client->socket = new_sd;
        client->addr = cl_addr;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, client) != 0) {
            perror("pthread_create");
            close(new_sd);
            free(client);
            continue;
        }

        // Detach per evitare memory leak (non serve join)
        pthread_detach(tid);
    }

    close(sd);
    return 0;
}
