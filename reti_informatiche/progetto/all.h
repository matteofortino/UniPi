#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef ALL
#define ALL
#define HOST "127.0.0.1"
#define LAVAGNA_PORT 5678

extern const char *commands[];
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

struct lavagna_info_t {
  int id;
  struct card_info_t *colonne[3];
};

// lavagna.c
int handle_input(char *, struct user_info_t *);
void *handle_user(void *);

// comandi.c
void QUIT(struct user_info_t *);
struct card_info_t *CREATE_CARD(struct user_info_t *, int, char *);
void add_card(struct lavagna_info_t *, struct card_info_t *);
void print_cards(struct lavagna_info_t *);

// lista_utenti.c
void add_user(int, struct sockaddr_in);
void remove_user(int port);
void print_users();

// utente.c
const char *match_command(const char *buffer);

#endif
