#include "common.h"

int my_port, lavagna_sock, my_server_sock;
int currently_working = 0;

void setup_p2p_server() {
  struct sockaddr_in addr;
  my_server_sock = socket(AF_INET, SOCK_STREAM, 0);
  int opt = 1;
  setsockopt(my_server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(my_port);
  bind(my_server_sock, (struct sockaddr *)&addr, sizeof(addr));
  listen(my_server_sock, 5);
}

void simulate_work_and_listen(int max_sec) {
  int duration = (rand() % max_sec) + 5; // Minimo 5 secondi
  currently_working = 1;
  printf("\n[WORK] Carico di lavoro assegnato. Esecuzione stimata: %d "
         "secondi...\n",
         duration);
  time_t start = time(NULL);
  while (difftime(time(NULL), start) < duration) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(lavagna_sock, &fds);
    struct timeval tv = {1, 0};
    if (select(lavagna_sock + 1, &fds, NULL, NULL, &tv) > 0) {
      Message m;
      if (recv_msg(lavagna_sock, &m) <= 0)
        exit(0);
      if (m.type == MSG_PING) {
        printf("[HEALTH-CHECK] Server Lavagna chiede: 'Sei vivo?'\n");
        if ((rand() % 100) < 15) { // 15% probabilità di ignorare
          printf("[DEBUG] Simulo un guasto hardware: ignoro la richiesta.\n");
        } else {
          printf("[HEALTH-CHECK] Rispondo: 'Sì, sto lavorando.' -> PONG\n");
          Message r;
          r.type = MSG_PONG;
          r.sender_port = my_port;
          send_msg(lavagna_sock, &r);
        }
      }
    }
    printf(".");
    fflush(stdout);
    sleep(1);
  }
  printf("\n[WORK] Task terminato correttamente!\n");
  currently_working = 0;
}

void handle_negotiation(Message *m_lavagna) {
  if (currently_working)
    return;

  int my_cost = rand() % 100;
  // Usiamo una costante per identificare il vincitore basata sulla porta in
  // caso di parità di costo
  printf("\n--- GARA P2P PER TASK %d ---\n", m_lavagna->card.id);
  printf("[LOG] Mio costo generato: %d\n", my_cost);

  // 1. INVIO COSTI A TUTTI GLI ALTRI
  for (int i = 0; i < m_lavagna->num_users; i++) {
    int target_port = m_lavagna->user_ports[i];
    if (target_port == my_port)
      continue; // Non auto-inviarsi il socket (già in memoria)

    int s = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in a;
    a.sin_family = AF_INET;
    a.sin_port = htons(target_port);
    inet_pton(AF_INET, LAVAGNA_IP, &a.sin_addr);

    // Timeout breve sulla connect per non bloccare il loop
    struct timeval tv = {0, 500000};
    setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    if (connect(s, (struct sockaddr *)&a, sizeof(a)) == 0) {
      Message out;
      memset(&out, 0, sizeof(out));
      out.type = MSG_CHOOSE_USER;
      out.sender_port = my_port;
      out.cost = my_cost;
      send_msg(s, &out);
      printf("[TX] Inviato costo %d a peer %d\n", my_cost, target_port);
    }
    close(s);
  }

  // 2. RACCOLTA VOTI
  int expected_votes = m_lavagna->num_users;
  int received_count = 1; // Il mio voto è il primo
  Message votes[MAX_USERS];
  votes[0].cost = my_cost;
  votes[0].sender_port = my_port;

  time_t start_wait = time(NULL);
  // Aspettiamo finché non abbiamo tutti i voti o scatta il timeout di 3 secondi
  while (received_count < expected_votes &&
         difftime(time(NULL), start_wait) < 3.0) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(my_server_sock, &fds);
    struct timeval tv = {0, 500000}; // Check ogni 0.5s

    if (select(my_server_sock + 1, &fds, NULL, NULL, &tv) > 0) {
      int ns = accept(my_server_sock, NULL, NULL);
      Message in;
      if (recv_msg(ns, &in) > 0) {
        printf("[RX] Ricevuto costo %d da peer %d\n", in.cost, in.sender_port);
        votes[received_count++] = in;
      }
      close(ns);
    }
  }

  // 3. CALCOLO VINCITORE (Solo se abbiamo ricevuto almeno qualche voto)
  printf("[LOG] Voti totali raccolti: %d/%d\n", received_count, expected_votes);

  int winner_port = votes[0].sender_port;
  int min_cost = votes[0].cost;

  for (int i = 1; i < received_count; i++) {
    // Se i costi sono uguali, vince la porta più bassa (determinismo)
    if (votes[i].cost < min_cost) {
      min_cost = votes[i].cost;
      winner_port = votes[i].sender_port;
    } else if (votes[i].cost == min_cost) {
      if (votes[i].sender_port < winner_port) {
        winner_port = votes[i].sender_port;
      }
    }
  }

  // 4. VERIFICA E NOTIFICA
  if (winner_port == my_port) {
    printf("[RISULTATO] HO VINTO LA GARA! (Costo: %d)\n", min_cost);
    Message ack;
    memset(&ack, 0, sizeof(ack));
    ack.type = MSG_ACK_CARD;
    ack.card = m_lavagna->card;
    ack.sender_port = my_port;
    send_msg(lavagna_sock, &ack);

    simulate_work_and_listen(20);

    Message d;
    d.type = MSG_CARD_DONE;
    d.card = m_lavagna->card;
    d.sender_port = my_port;
    send_msg(lavagna_sock, &d);
  } else {
    printf("[RISULTATO] Gara persa. Il vincitore è %d\n", winner_port);
  }
  printf("> ");
  fflush(stdout);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Esegui: ./utente <tua_porta>\n");
    return -1;
  }
  my_port = atoi(argv[1]);
  srand(time(NULL) + my_port);
  setup_p2p_server();
  struct sockaddr_in addr;
  lavagna_sock = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(LAVAGNA_PORT);
  inet_pton(AF_INET, LAVAGNA_IP, &addr.sin_addr);
  if (connect(lavagna_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Lavagna non attiva");
    exit(1);
  }

  Message h;
  h.type = MSG_HELLO;
  h.sender_port = my_port;
  send_msg(lavagna_sock, &h);
  printf("=== CLIENT KANBAN ATTIVO (Porta P2P: %d) ===\n", my_port);
  printf("Comandi: 'add [id] [descrizione]', 'list' (vedi board), 'quit' "
         "(esci)\n> ");
  fflush(stdout);

  while (1) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(lavagna_sock, &fds);
    FD_SET(STDIN_FILENO, &fds);
    select(lavagna_sock + 1, &fds, NULL, NULL, NULL);
    if (FD_ISSET(STDIN_FILENO, &fds)) {
      char buf[256];
      fgets(buf, 256, stdin);
      if (strncmp(buf, "list", 4) == 0) {
        printf("[REQ] Richiesta stato board al server...\n");
        Message m;
        m.type = MSG_REQ_LIST;
        send_msg(lavagna_sock, &m);
      } else if (strncmp(buf, "add", 3) == 0) {
        int id;
        char desc[128];
        // Formato atteso: add <id> <descrizione>
        if (sscanf(buf + 4, "%d %[^\n]", &id, desc) == 2) {
          printf("[SEND] Tentativo di creazione card ID %d: '%s'...\n", id,
                 desc);
          Message m;
          memset(&m, 0, sizeof(m));
          m.type = MSG_ADD_CARD;
          m.card.id = id;
          strncpy(m.card.text, desc, 255);
          send_msg(lavagna_sock, &m);
        } else {
          printf("[ERROR] Formato errato. Usa: add <id> <descrizione>\n> ");
        }
      } else if (strncmp(buf, "quit", 4) == 0) {
        printf("[INFO] Chiusura in corso...\n");
        exit(0);
      } else {
        printf("> ");
      }
      fflush(stdout);
    }
    if (FD_ISSET(lavagna_sock, &fds)) {
      Message m;
      if (recv_msg(lavagna_sock, &m) <= 0) {
        printf("\n[ERR] Connessione persa con il server Lavagna.\n");
        break;
      }

      switch (m.type) {
      case MSG_AVAILABLE_CARD:
        handle_negotiation(&m);
        break;

      case MSG_RES_LIST:
        printf("\n%s\n> ", m.payload);
        break;

      case MSG_SUCCESS:
        printf("\n[CONFERMA SERVER] %s\n> ", m.payload);
        break;

      case MSG_ERROR:
        printf("\n[ERRORE SERVER] %s\n> ", m.payload);
        break;

      case MSG_PING:
        // Gestione del ping quando non si è nel ciclo di simulate_work
        printf(
            "\n[HEALTH-CHECK] Ricevuto PING in stato idle. Rispondo PONG.\n> ");
        Message r;
        memset(&r, 0, sizeof(r));
        r.type = MSG_PONG;
        r.sender_port = my_port;
        send_msg(lavagna_sock, &r);
        break;

      default:
        printf("\n[DEBUG] Ricevuto messaggio tipo %d non gestito.\n> ", m.type);
        break;
      }
      fflush(stdout);
    }
  }
  return 0;
}
