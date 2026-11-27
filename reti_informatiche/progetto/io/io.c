#include "io.h"
#include "../common/common.h"
#include <errno.h>
#include <string.h>

IoStatus send_all(int fd, const void *data, size_t len) {
    const unsigned char *p = data;
    size_t sent = 0;
    while (sent < len) {
        ssize_t n = send(fd, p + sent, len - sent, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return IO_ERR;
        }
        if (n == 0)
            return IO_EOF;
        sent += (size_t)n;
    }
    return IO_OK;
}

IoStatus recv_exact(int fd, void *data, size_t len) {
    unsigned char *p = data;
    size_t got = 0;
    while (got < len) {
        ssize_t n = recv(fd, p + got, len - got, 0);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            return IO_ERR;
        }
        if (n == 0)
            return IO_EOF;
        got += (size_t)n;
    }
    return IO_OK;
}

IoStatus recv_line(int fd, char *data, size_t capacity) {
    unsigned char byte;
    int n_char = 0;

    if (data == NULL || capacity < MAX_LINE_LENGTH)
        return IO_BAD_PARAM;

    while (1) {

        int ret = recv_exact(fd, &byte, 1);

        if (ret == IO_EOF)
            return IO_EOF;

        if (ret < 0)
            return IO_ERR;

        if (byte == '\n') {
            data[n_char] = '\0';
            return IO_OK;
        }

        if (byte < 0x20 || byte > 0x7e)
            return IO_BAD_CHAR;

        if (n_char >= MAX_LINE_LENGTH - 1)
            return IO_LINE_TOO_LONG;

        data[n_char++] = (char)byte;
    }
}

IoStatus send_error(int fd, ProtocolError err) {
    const char *err_msg = protocol_error_str(err);

    return send_all(fd, err_msg, strlen(err_msg));
}
