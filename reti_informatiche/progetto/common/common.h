#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdint.h>

#define LOCALHOST "127.0.0.1"
#define MAX_LINE_LENGTH 512
#define MAX_FIELDS 6
#define MAX_NAME_LENGTH 64
#define MAX_FILE_SIZE (10u * 1024u * 1024u) // 10 485 760 bytes

typedef enum {
    IO_OK = 0,
    IO_EOF = -1,           // ex CONNECTION_CLOSED
    IO_ERR = -2,           // ex LINE_RECV_FAILED / errore send o recv
    IO_LINE_TOO_LONG = -3, // >= 512 byte senza '\n' -> il chiamante risponde ERR BAD_REQUEST
    IO_BAD_CHAR = -4,      // byte non ASCII valido -> il chiamante risponde ERR BAD_REQUEST
    IO_BAD_PARAM = -5      // ex ERR_BAD_PARAMETERS
} IoStatus;

typedef enum {
    PROTO_OK = 0,
    PROTO_ERR_BAD_REQUEST,  // "ERR BAD_REQUEST\n"
    PROTO_ERR_INVALID_NAME, // "ERR INVALID_NAME\n"
    PROTO_ERR_INVALID_SIZE, // "ERR INVALID_SIZE\n"
    PROTO_ERR_INVALID_PORT, // "ERR INVALID_PORT\n"
    PROTO_ERR_NO_STORAGE,   // "ERR NO_STORAGE\n"
    PROTO_ERR_FILE_EXISTS,  // "ERR FILE_EXISTS\n"
    PROTO_ERR_NOT_FOUND,    // "ERR NOT_FOUND\n"
    PROTO_ERR_IO            // "ERR IO\n"
} ProtocolError;

typedef enum {
    CMD_UNKNOWN = -1, // Unknown command
    CMD_REGISTER,     // REGISTER <port>
    CMD_STORED,       // STORED <remote name> <size>
    CMD_PUT,          // PUT <remote name> <size>
    CMD_GET,          // GET <remote name>
    CMD_LIST,         // LIST
    CMD_UPLOAD,       // UPLOAD <remote name> <size>
    CMD_DOWNLOAD      // DOWNLOAD <remote name>
} Command;

typedef struct {
    char remote_name[MAX_NAME_LENGTH + 1];
    uint16_t storage_port;
    size_t size;
    Command command;
} Request;

ProtocolError split_fields(char *line, char **fields, int *count_out);
ProtocolError validate_remote_name(char *remote_name);
const char *protocol_error_str(ProtocolError err);
ProtocolError parse_request(char **fields, int count, Request *out);
ProtocolError parse_port(const char *s, uint16_t *out);
#endif
