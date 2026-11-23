#include "io.h"

int send_all(int fd, const void *data, size_t len) {
    const unsigned char *p = data;
    size_t sent = 0;

    while (sent < len) {
        ssize_t n = send(fd, p + sent, len - sent, MSG_NOSIGNAL);

        if (n < 0) {
            if (errno == EINTR)
                continue; // interrotta: riprova
            return -1;
        }
        if (n == 0)
            return -1;

        sent += (size_t)n;
    }
    return 0;
}

int recv_exact(int fd, void *data, size_t len) {
    unsigned char *p = data;
    size_t received = 0;

    while (received < len) {
        ssize_t n = recv(fd, p + received, len - received, 0);

        if (n < 0) {
            if (errno == EINTR)
                continue;
            return -1;
        }
        if (n == 0)
            return -1; // EOF prima di aver ricevuto tutti i byte

        received += (size_t)n;
    }
    return 0;
}

int recv_control_msg(int fd, void *data) {
    char *p = data;
    int i = 0;
    while (1) {
        if (recv_exact(fd, p, 1) < 0)
            return -1;

        if (*p == '\n')
            return 0;
        p++;

        if (i > 256)
            return -1;
        i++;
    }

    return -1;
}

// come utlizzarla per inviare dai file
//
// unsigned char buf[4096];
// uint64_t remaining = size;
//
// while (remaining > 0) {
//     size_t want = remaining < sizeof buf ? (size_t)remaining : sizeof buf;
//     size_t n = fread(buf, 1, want, file);
//
//     if (n == 0) return -1;  // errore o file terminato prima del previsto
//     if (send_all(fd, buf, n) < 0) return -1;
//
//     remaining -= n;
// }
//
// come utlizzrla per ricevere file
//
// uint64_t remaining = size;
// while (remaining > 0) {
//     size_t chunk = remaining < sizeof buf ? (size_t)remaining : sizeof buf;
//
//     if (recv_exact(fd, buf, chunk) < 0) return -1;
//     if (fwrite(buf, 1, chunk, file) != chunk) return -1;
//
//     remaining -= chunk;
// }
