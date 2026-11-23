#ifndef IO_H
#define IO_H
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>

// helper function since the standard send and recv
// do not guarantee to send nor receive all the bytes at once

int send_all(int fd, const void *data, size_t len);

int recv_exact(int fd, void *data, size_t len);

int recv_control_msg(int fd, void *data);
#endif
