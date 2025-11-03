#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
int main (int argc, char *argv[]) {
    int port = atoi(argv[1]);

    int ret, sd, new_sd, len;
    struct sockaddr_in my_addr, cl_addr; // Due strutture!
    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("socket");
        return 1;
    }
    /* Creazione indirizzo */
    memset(&my_addr, 0, sizeof(my_addr)); // Pulizia
    my_addr.sin_family = AF_INET ;
    my_addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &my_addr.sin_addr);
    // In alternativa socket in ascolto su tutte le interfacce:
    // my_addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(sd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if(ret < 0) {
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
    len = sizeof(cl_addr);

    // endless loop to accept connections
    while(1) {
        new_sd = accept(sd, (struct sockaddr*)&cl_addr, (socklen_t *)&len);
        if (new_sd < 0) {
            perror("accept");
            close(sd);
            return 1;
        }
        printf("Client connesso...\n");
        char buffer[20];
        char message[20];
        memset(message, 0, sizeof(message));
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(new_sd, buffer, sizeof(buffer), 0);
        if(bytes < 0) {
            perror("recv");
            break;
        }
        strcpy(message, "Hello client!");
        printf("Received message from client: %s\n", buffer);
        send(new_sd, message, sizeof(message), 0);
        close(new_sd);
        printf("Connessione chiusa...\n\n");
    }
    close(new_sd);
    close(sd);
}
