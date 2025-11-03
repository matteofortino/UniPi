#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main (int argc, char *argv[]) {
    int port = atoi(argv[2]);
    char *host = argv[1];
    int ret, sd;
    struct sockaddr_in sv_addr; // Struttura per il server
    /* Creazione socket */
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0) {
        perror("socket");
        return 1;
    }

    /* Creazione indirizzo del server */
    memset(&sv_addr, 0, sizeof(sv_addr)); // Pulizia
    sv_addr.sin_family = AF_INET ;
    sv_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &sv_addr.sin_addr);
    ret = connect(sd, (struct sockaddr*)&sv_addr,sizeof(sv_addr));
    if(ret < 0) {
        perror("connect");
        close(sd);
        return 1;
    }
    printf("Connessione riuscita...\n");
    char message[20];
    char buffer[20];
    memset(message, 0, sizeof(message));
    memset(buffer, 0, sizeof(buffer));

    strcpy(message, "Hello server!");
    send(sd, message, sizeof(message), 0);

    int bytes = recv(sd, buffer, sizeof(buffer), 0);
    if(bytes < 0) {
        perror("recv");
        close(sd);
        return 1;
    }
    printf("Received message from server: %s\n", buffer);
    close(sd);
    printf("Connesione chiusa...\n");
}
