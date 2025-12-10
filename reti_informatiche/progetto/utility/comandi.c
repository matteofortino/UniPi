#include "./lista_utenti.c"
// lista dei comandi disponibili (matricola pari)
const char *commands[] = {
    "HELLO",          "CREATE_CARD",    "QUIT",      "MOVE_CARD",
    "SHOW_LAVAGNA",   "SEND_USER_LIST", "PING_USER", "PONG_LAVAGNA",
    "AVAILABLE_CARD", "CHOOSE_USER",    "ACK_CARD",  "CARD_DONE",
};

// funzione che gestisce il comando QUIT
void QUIT(struct user_info_t *user) {
  printf("User [%d] disconnected\n", ntohs(user->addr.sin_port));
  remove_user(ntohs(user->addr.sin_port));
  print_users();
  close(user->socket);
  free(user);
}
int id = 0;
void CREATE_CARD(int port) {
  struct card_info_t *card = malloc(sizeof(struct card_info_t));
  card->id = id++;
  card->state = TODO;
  char buffer[1024];
  printf("Insierisci una descrizione per la card: ");
  fgets(buffer, sizeof(buffer), stdin);
  buffer[strlen(buffer) - 1] = '\0';
  strcpy(card->description, buffer);
  card->utente = port;
  card->timestamp = time(NULL);
}
