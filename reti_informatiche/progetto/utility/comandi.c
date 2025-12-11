#include "../all.h"
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
}

// funzione per la creazione di una nuova card
struct card_info_t *CREATE_CARD(struct user_info_t *user, int id,
                                char *description) {
  struct card_info_t *card = malloc(sizeof(struct card_info_t));
  card->id = id;
  card->state = TODO;
  card->utente = ntohs(user->addr.sin_port);
  card->timestamp = time(NULL);
  strcpy(card->description, description);

  return card;
}
void add_card(struct lavagna_info_t *lavagna, struct card_info_t *card) {
  const int state = card->state;
  if (lavagna->colonne[state] == NULL)
    lavagna->colonne[state] = card;
  else {
    struct card_info_t *current = lavagna->colonne[state];
    while (current->next != NULL)
      current = current->next;
    current->next = card;
  }
}

void print_cards(struct lavagna_info_t *lavagna) {
  for (int state = TODO; state <= DONE; state++) {
    struct card_info_t *card = lavagna->colonne[state];
    while (card != NULL) {
      printf(
          "Card ID: %d\nDescription: %s\nSTATE: %d\nUser: %d\nTimestamp: %d\n",
          card->id, card->description, card->state, card->utente,
          card->timestamp);
      card = card->next;
    }
  }
}
