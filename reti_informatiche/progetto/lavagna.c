#include "common.h"

Card cards[MAX_CARDS];
int num_cards = 0;

typedef struct {
  int socket;
  int port;
  int active;
  int is_working;
  time_t task_start_time;
  time_t ping_sent_time;
  int pong_received;
} ConnectedUser;

ConnectedUser users[MAX_USERS];
int num_users_connected = 0;

void serialize_lavagna(char *buffer) {
  memset(buffer, 0, BUF_SIZE);
  sprintf(buffer,
          "\n==================== KANBAN DASHBOARD ====================\n");
  sprintf(buffer + strlen(buffer), "Utenti attivi: %d | Task totali: %d\n",
          num_users_connected, num_cards);
  strcat(buffer,
         "----------------------------------------------------------\n");

  strcat(buffer, "[TO DO] (In attesa di assegnazione):\n");
  for (int i = 0; i < num_cards; i++)
    if (cards[i].status == ST_TODO)
      sprintf(buffer + strlen(buffer), "  ID %02d: %-35s\n", cards[i].id,
              cards[i].text);

  strcat(buffer, "\n[DOING] (In fase di sviluppo):\n");
  for (int i = 0; i < num_cards; i++)
    if (cards[i].status == ST_DOING)
      sprintf(buffer + strlen(buffer), "  ID %02d: %-35s -> [Dev: %d]\n",
              cards[i].id, cards[i].text, cards[i].owner_port);

  strcat(buffer, "\n[DONE] (Completati):\n");
  for (int i = 0; i < num_cards; i++)
    if (cards[i].status == ST_DONE)
      sprintf(buffer + strlen(buffer), "  ID %02d: %-35s\n", cards[i].id,
              cards[i].text);
  strcat(buffer,
         "==========================================================\n");
}

void show_lavagna_console() {
  printf("\033[H\033[J");
  char buff[BUF_SIZE];
  serialize_lavagna(buff);
  printf("%s", buff);
  printf("[SYS-LOG] In attesa di segnali dai peer...\n");
  fflush(stdout);
}

void check_and_broadcast_task() {
  if (num_users_connected < 2) {
    printf("[INFO] Almeno 2 utenti richiesti per la gara. Attendo altri "
           "peer...\n");
    return;
  }
  int idx = -1;
  for (int i = 0; i < num_cards; i++)
    if (cards[i].status == ST_TODO) {
      idx = i;
      break;
    }
  if (idx == -1)
    return;

  printf("[EVENT] Task ID %d disponibile. Invio notifica di negoziazione...\n",
         cards[idx].id);
  Message msg;
  memset(&msg, 0, sizeof(msg));
  msg.type = MSG_AVAILABLE_CARD;
  msg.card = cards[idx];
  for (int i = 0; i < MAX_USERS; i++)
    if (users[i].active)
      msg.user_ports[msg.num_users++] = users[i].port;
  for (int i = 0; i < MAX_USERS; i++)
    if (users[i].active)
      send_msg(users[i].socket, &msg);
}

void release_user_card(int user_port) {
  int changed = 0;
  for (int i = 0; i < num_cards; i++) {
    if (cards[i].owner_port == user_port && cards[i].status == ST_DOING) {
      printf("[RECOVERY] Task ID %d rimosso da utente %d e rimesso in TODO.\n",
             cards[i].id, user_port);
      cards[i].status = ST_TODO;
      cards[i].owner_port = -1;
      changed = 1;
    }
  }
  if (changed) {
    show_lavagna_console();
    check_and_broadcast_task();
  }
}

void manage_timeouts() {
  time_t now = time(NULL);
  for (int i = 0; i < MAX_USERS; i++) {
    if (users[i].active && users[i].is_working) {

      // CONDIZIONE 1: Invia il PING solo se sono passati 30s e NON c'è già un
      // ping pendente
      if (users[i].ping_sent_time == 0 &&
          difftime(now, users[i].task_start_time) > 30.0) {
        printf("[NETWORK] Timer scaduto per %d. Invio PING di controllo...\n",
               users[i].port);
        Message p;
        memset(&p, 0, sizeof(p));
        p.type = MSG_PING;
        send_msg(users[i].socket, &p);

        users[i].ping_sent_time = now; // Segniamo che stiamo aspettando
        users[i].pong_received = 0;    // Resettiamo il flag di risposta
      }

      // CONDIZIONE 2: Se abbiamo inviato un PING, aspettiamo il PONG per max 5
      // secondi
      else if (users[i].ping_sent_time > 0 && !users[i].pong_received) {
        if (difftime(now, users[i].ping_sent_time) > 5.0) {
          printf("[ALERT] TIMEOUT! L'utente %d non ha risposto al PING entro "
                 "5s. Disconnessione.\n",
                 users[i].port);
          int p_lost = users[i].port;
          close(users[i].socket);
          users[i].active = 0;
          num_users_connected--;
          release_user_card(p_lost);
        }
      }
    }
  }
}

int main() {
  char *titles[] = {
      "Fix bug login OAuth2",        "Refactor API endpoint /user",
      "Setup Docker Swarm",          "Deploy Kubernetes Cluster",
      "Implementazione Redis Cache", "Ottimizzazione Query SQL",
      "Unit Test Modulo Pagamenti",  "Integrazione Stripe API",
      "Documentazione OpenAPI 3.0",  "Configurazione Nginx Ingress"};
  for (int i = 0; i < 10; i++) {
    cards[i].id = i + 1;
    strncpy(cards[i].text, titles[i], 255);
    cards[i].status = ST_TODO;
    cards[i].owner_port = -1;
  }
  num_cards = 10;

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;
  int opt = 1;
  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(LAVAGNA_PORT);
  bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
  listen(server_fd, MAX_USERS);

  show_lavagna_console();

  while (1) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_fd, &readfds);
    int max_sd = server_fd;
    for (int i = 0; i < MAX_USERS; i++)
      if (users[i].active) {
        FD_SET(users[i].socket, &readfds);
        if (users[i].socket > max_sd)
          max_sd = users[i].socket;
      }

    struct timeval tv = {1, 0};
    select(max_sd + 1, &readfds, NULL, NULL, &tv);
    manage_timeouts();

    if (FD_ISSET(server_fd, &readfds)) {
      int new_s = accept(server_fd, NULL, NULL);
      for (int i = 0; i < MAX_USERS; i++)
        if (!users[i].active) {
          users[i].socket = new_s;
          users[i].active = 1;
          users[i].is_working = 0;
          users[i].ping_sent_time = 0;
          printf("[CONN] Nuovo peer connesso nel socket %d\n", new_s);
          num_users_connected++;
          break;
        }
    }

    for (int i = 0; i < MAX_USERS; i++) {
      if (users[i].active && FD_ISSET(users[i].socket, &readfds)) {
        Message msg;
        if (recv_msg(users[i].socket, &msg) <= 0) {
          printf("[DISCONN] Peer %d ha chiuso la connessione.\n",
                 users[i].port);
          int p_lost = users[i].port;
          close(users[i].socket);
          users[i].active = 0;
          num_users_connected--;
          release_user_card(p_lost);
          show_lavagna_console();
        } else {
          switch (msg.type) {
          case MSG_HELLO:
            users[i].port = msg.sender_port;
            printf("[HELLO] Peer registrato sulla porta P2P: %d\n",
                   users[i].port);
            show_lavagna_console();
            check_and_broadcast_task();
            break;
          case MSG_PONG:
            if (users[i].ping_sent_time > 0) {
              printf("[NETWORK] PONG ricevuto da %d. Sistema integro.\n",
                     users[i].port);
              users[i].pong_received = 1;
              users[i].ping_sent_time =
                  0; // IMPORTANTE: Resetta lo stato di attesa
              users[i].task_start_time =
                  time(NULL); // Riavvia il countdown dei 30s da ora
            }
            break;
          case MSG_ACK_CARD:
            printf("[ACK] Peer %d ha vinto la gara per il task ID %d.\n",
                   msg.sender_port, msg.card.id);
            for (int c = 0; c < num_cards; c++)
              if (cards[c].id == msg.card.id && cards[c].status == ST_TODO) {
                cards[c].status = ST_DOING;
                cards[c].owner_port = msg.sender_port;
                users[i].is_working = 1;
                users[i].task_start_time = time(NULL);
                users[i].ping_sent_time = 0;
              }
            show_lavagna_console();
            break;
          case MSG_CARD_DONE:
            printf("[DONE] Task ID %d completato da peer %d.\n", msg.card.id,
                   msg.sender_port);
            users[i].is_working = 0;
            users[i].ping_sent_time = 0;
            for (int c = 0; c < num_cards; c++)
              if (cards[c].id == msg.card.id)
                cards[c].status = ST_DONE;
            show_lavagna_console();
            check_and_broadcast_task();
            break;
          case MSG_REQ_LIST:
            printf(
                "[INFO] Peer %d ha richiesto la visualizzazione della board.\n",
                users[i].port);
            Message r;
            r.type = MSG_RES_LIST;
            serialize_lavagna(r.payload);
            send_msg(users[i].socket, &r);
            break;
          case MSG_ADD_CARD: {
            printf("[ADD-REQ] Ricevuta richiesta per ID %d da peer %d.\n",
                   msg.card.id, users[i].port);

            // Controllo se l'ID esiste già
            int exists = 0;
            for (int j = 0; j < num_cards; j++) {
              if (cards[j].id == msg.card.id) {
                exists = 1;
                break;
              }
            }

            Message response;
            memset(&response, 0, sizeof(Message));

            if (exists) {
              response.type = MSG_ERROR;
              sprintf(response.payload, "L'ID %d è già presente nella board!",
                      msg.card.id);
              send_msg(users[i].socket, &response);
            } else {
              cards[num_cards] = msg.card;
              cards[num_cards].status = ST_TODO;
              cards[num_cards].owner_port = -1;
              num_cards++;

              response.type = MSG_SUCCESS;
              sprintf(response.payload, "Task '%s' aggiunto con ID %d.",
                      msg.card.text, msg.card.id);
              send_msg(users[i].socket, &response);

              show_lavagna_console();
              check_and_broadcast_task(); // Tenta di avviare una gara se ci
                                          // sono abbastanza peer
            }
          } break;
          default:
            break;
          }
        }
      }
    }
  }
}
