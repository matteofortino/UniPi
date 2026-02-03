#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#define LAVAGNA_PORT 5678
#define LAVAGNA_IP "127.0.0.1"
#define MAX_USERS 10
#define MAX_CARDS 50
#define BUF_SIZE 4096

typedef enum { ST_TODO, ST_DOING, ST_DONE } CardStatus;

typedef struct {
  int id;
  CardStatus status;
  char text[256];
  int owner_port;
} Card;

typedef enum {
  MSG_HELLO,
  MSG_AVAILABLE_CARD,
  MSG_CHOOSE_USER,
  MSG_ACK_CARD,
  MSG_CARD_DONE,
  MSG_REQ_LIST,
  MSG_RES_LIST,
  MSG_ADD_CARD,
  MSG_PING,
  MSG_PONG,
  MSG_QUIT,
  MSG_ERROR = 100,
  MSG_SUCCESS = 101
} MessageType;

typedef struct {
  MessageType type;
  int sender_port;
  Card card;
  int cost;
  int user_ports[MAX_USERS];
  int num_users;
  char payload[BUF_SIZE];
} Message;

static inline int send_msg(int sock, Message *msg) {
  return send(sock, msg, sizeof(Message), 0);
}

static inline int recv_msg(int sock, Message *msg) {
  return recv(sock, msg, sizeof(Message), 0);
}

#endif
