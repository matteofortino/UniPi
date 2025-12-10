/*
 * Questo file contiene le funzioni per la gestione della lista degli utenti
 * connessi alla lavagna. la add_user e' la funzione di registrazione di un
 * nuovo utente corrispondente al comando HELLO; la remove_user e' la funzione
 * di deregistrazione di un utente corrispondente al comando QUIT;
 */
#pragma once
#include "../common.h"
int num_users = 0;

// mutex per gli inserimenti e le rimozioni dalla lista degli utenti
pthread_mutex_t users_mutex = PTHREAD_MUTEX_INITIALIZER;

struct user_info_t *users = NULL;
void add_user(int sock, struct sockaddr_in addr) {
  struct user_info_t *u = malloc(sizeof(struct user_info_t));
  if (!u) {
    perror("malloc");
    return;
  }
  u->socket = sock;
  u->activity = NULL;
  u->addr = addr;

  pthread_mutex_lock(&users_mutex);
  u->next = (struct user_info_t *)users;
  users = u;
  num_users++;
  pthread_mutex_unlock(&users_mutex);
}

// TODO: se l'utente ha un card in mano quando viene rimosso la card  torno
// nella colonna todo
void remove_user(int port) {
  pthread_mutex_lock(&users_mutex);

  struct user_info_t **curr = &users;
  while (*curr) {
    if (ntohs((*curr)->addr.sin_port) == port) {
      struct user_info_t *tmp = *curr;
      *curr = ((*curr)->next);
      free(tmp);
      break;
    }
    curr = &((*curr)->next);
  }
  num_users--;
  pthread_mutex_unlock(&users_mutex);
}

void print_users() {
  pthread_mutex_lock(&users_mutex);

  struct user_info_t *u = users;
  printf("=== Connected Users ===\n");
  while (u) {
    printf("Socket: %d | IP: %s | Port: %d\n", u->socket,
           inet_ntoa(u->addr.sin_addr), ntohs(u->addr.sin_port));
    u = u->next;
  }
  printf("========================\n");
  printf("Users number: %d\n", num_users);
  pthread_mutex_unlock(&users_mutex);
}
