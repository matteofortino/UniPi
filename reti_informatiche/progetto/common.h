#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define HOST "127.0.0.1"
#define LAVAGNA_PORT 5678

enum lavagna_states { TODO = 0, DOING = 1, DONE = 2 };

// lista delgi utenti connessi alla lavagna
struct user_info_t {
  int socket;
  char *activity;
  struct sockaddr_in addr;
  struct user_info_t *next;
};

// TODO: FINIRE DI IMPLEMENTARE
struct card_info_t {
  int id;
  enum lavagna_states state;
  char description[1024];
  int utente;
  int timestamp;
  struct card_info_t *next;
};
