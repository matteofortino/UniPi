#include "all.h"
#include <stdio.h>

#define NUM_COMMANDS 12
// Confronta buffer con i comandi. Restituisce il comando trovato o NULL
const char *match_command(const char *buffer) {
  if (!buffer)
    return NULL;

  for (int i = 0; i < NUM_COMMANDS; i++) {
    if (strcmp(buffer, commands[i]) == 0) {
      return commands[i];
    }
  }

  return NULL; // nessun comando trovato
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: %s <arg>\n", argv[0]);
    return 1;
  }

  int port = atoi(argv[1]);
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in server_addr, utente_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  memset(&utente_addr, 0, sizeof(utente_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LAVAGNA_PORT);
  inet_pton(AF_INET, HOST, &server_addr.sin_addr);

  utente_addr.sin_family = AF_INET;
  utente_addr.sin_port = htons(port);
  inet_pton(AF_INET, HOST, &utente_addr.sin_addr);

  if (bind(sockfd, (struct sockaddr *)&utente_addr, sizeof(utente_addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  int ret = connect(sockfd, (struct sockaddr *)&server_addr,
                    (socklen_t)sizeof(server_addr));
  if (ret < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  char buffer[1024];
  while (1) {
    printf("Enter message: ");
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strlen(buffer) - 1] = '\0';
    if (match_command(buffer) == NULL) {
      printf("Invalid command\n");
      continue;
    }
    send(sockfd, buffer, strlen(buffer), 0);

    if (strcmp(buffer, "QUIT") == 0) {
      printf("Connection closed.\n");
      close(sockfd);
      exit(EXIT_SUCCESS);
    }
    if (strcmp(buffer, "CREATE_CARD") == 0) {
      // invio dell'id
      memset(buffer, 0, sizeof(buffer));
      recv(sockfd, buffer, sizeof(buffer), 0);
      printf("%s", buffer);
      fgets(buffer, sizeof(buffer), stdin);
      buffer[strlen(buffer) - 1] = '\0';
      send(sockfd, buffer, strlen(buffer), 0);

      // invio della descrizione
      memset(buffer, 0, sizeof(buffer));
      recv(sockfd, buffer, sizeof(buffer), 0);
      printf("%s", buffer);
      fgets(buffer, sizeof(buffer), stdin);
      buffer[strlen(buffer) - 1] = '\0';
      send(sockfd, buffer, strlen(buffer), 0);
    }
  }
}
