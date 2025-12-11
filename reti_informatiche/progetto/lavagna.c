#include "all.h"

#include <stdlib.h>

struct lavagna_info_t *lavagna;
int handle_input(char *buffer, struct user_info_t *user) {
  if (strcmp(buffer, "QUIT") == 0) {
    QUIT(user);
    return 0;
  }
  if (strcmp(buffer, "CREATE_CARD") == 0) {
    // chiedo all'utente l'id della card
    strcpy(buffer, "[lavagna] Card ID: ");
    send(user->socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(user->socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      perror("recv");
      return -1;
    }
    int id = atoi(buffer);

    // TODO: verificare che l'id sia univoco

    // chiedo all'utente la descrizione della card
    strcpy(buffer, "[lavagna] Card Description: ");
    send(user->socket, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = recv(user->socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      perror("recv");
      return -1;
    }
    char description[BUFFER_SIZE];
    memset(description, 0, sizeof(description));

    memcpy(description, buffer, bytes_received);

    // creo la card
    struct card_info_t *card = CREATE_CARD(user, id, description);
    add_card(lavagna, card);
    strcpy(buffer, "[lavagna] Card Created Successfully");

    print_cards(lavagna);
    return 1;
  }
  return 1;
}

void *handle_user(void *arg) {
  struct user_info_t *user = (struct user_info_t *)arg;

  add_user(user->socket, user->addr);
  print_users();
  char buffer[BUFFER_SIZE];
  while (1) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytes_received = recv(user->socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
      perror("recv");
      break;
    }
    if (bytes_received == 0) {
      printf("User [%d] disconnected\n", ntohs(user->addr.sin_port));
      remove_user(ntohs(user->addr.sin_port));
      print_users();
      break;
    }

    printf("Received message from user [%d]: %s\n", ntohs(user->addr.sin_port),
           buffer);

    int ret = handle_input(buffer, user);
    if (ret == 0)
      break;

    // send(user_info->socket, buffer, bytes_received, 0);
  }

  close(user->socket);
  free(user);
  return NULL;
}

int main() {
  struct sockaddr_in server_addr, user_addr;

  lavagna = malloc(sizeof(struct lavagna_info_t));
  lavagna->id = 0;

  lavagna->colonne[TODO] = NULL;
  lavagna->colonne[DOING] = NULL;
  lavagna->colonne[DONE] = NULL;

  memset(&server_addr, 0, sizeof(server_addr));
  memset(&user_addr, 0, sizeof(user_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(LAVAGNA_PORT);
  inet_pton(AF_INET, HOST, &server_addr.sin_addr);

  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(sockfd, 5) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  printf("Lavagna in ascolto sulla porta %d\n", LAVAGNA_PORT);
  while (1) {
    socklen_t user_addr_len = sizeof(user_addr);
    int user_sockfd =
        accept(sockfd, (struct sockaddr *)&user_addr, &user_addr_len);
    if (user_sockfd < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    struct user_info_t *user = malloc(sizeof(struct user_info_t));
    user->socket = user_sockfd;
    user->addr = user_addr;

    pthread_t tid;
    pthread_create(&tid, NULL, handle_user, (void *)user);
    pthread_detach(tid);
  }

  close(sockfd);
}
