#ifndef IO_H
#define IO_H
#include <stddef.h>
#include <sys/socket.h>
#include "../common/common.h"

IoStatus send_all(int fd, const void *data, size_t len);
IoStatus recv_exact(int fd, void *data, size_t len);
IoStatus recv_line(int fd, char *data, size_t capacity);
IoStatus send_error(int fd, ProtocolError err);

#endif
