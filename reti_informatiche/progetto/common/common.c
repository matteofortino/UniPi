#include "common.h"
#include <stdio.h>
#include <string.h>

//@param remote_name char*
//@return int on success returns 0 on fail returns < 0
ProtocolError validate_remote_name(char *remote_name) {

    if (remote_name == NULL)
        return PROTO_ERR_INVALID_NAME;

    size_t length = strlen(remote_name);

    if (length < 1 || length > MAX_NAME_LENGTH)
        return PROTO_ERR_INVALID_NAME;

    if (strcmp(remote_name, ".") == 0 || strstr(remote_name, "..") != NULL)
        return PROTO_ERR_INVALID_NAME;

    if (length >= 5 && strcmp(remote_name + length - 5, ".part") == 0)
        return PROTO_ERR_INVALID_NAME;

    for (size_t i = 0; i < length; i++) {
        unsigned char c = (unsigned char)remote_name[i];
        int ok = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
                 c == '.' || c == '_' || c == '-';
        if (!ok)
            return PROTO_ERR_INVALID_NAME;
    }

    return PROTO_OK;
}

//@param line char* line to be parsed
//@param fields char** array of substring in line
//@return int number of fields in the line
ProtocolError split_fields(char *line, char **fields, int *count_out) {
    if (line == NULL || fields == NULL)
        return PROTO_ERR_BAD_REQUEST;

    if (*line == '\0')
        return PROTO_ERR_BAD_REQUEST;

    char *field_start = line;
    size_t count = 0;

    for (char *p = line;; p++) {

        if (*p != '\0' && *p != ' ') {
            // controllo se sono caratteri ASCII scrivibili
            if (*p < 0x21 || *p > 0x7e)
                return PROTO_ERR_BAD_REQUEST;
            continue;
        }

        if (p == field_start || count >= MAX_FIELDS)
            return PROTO_ERR_BAD_REQUEST;

        fields[count++] = field_start;

        if (*p == '\0')
            break;

        *p = '\0';
        field_start = p + 1;
    }
    *count_out = (int)count;
    return PROTO_OK;
}

const char *protocol_error_str(ProtocolError err) {
    switch (err) {
    case PROTO_ERR_BAD_REQUEST:
        return "ERR BAD_REQUEST\n";
    case PROTO_ERR_INVALID_NAME:
        return "ERR INVALID_NAME\n";
    case PROTO_ERR_INVALID_SIZE:
        return "ERR INVALID_SIZE\n";
    case PROTO_ERR_INVALID_PORT:
        return "ERR INVALID_PORT\n";
    case PROTO_ERR_NO_STORAGE:
        return "ERR NO_STORAGE\n";
    case PROTO_ERR_FILE_EXISTS:
        return "ERR FILE_EXISTS\n";
    case PROTO_ERR_NOT_FOUND:
        return "ERR NOT_FOUND\n";
    case PROTO_ERR_IO:
        return "ERR IO\n";
    case PROTO_OK:
    default:
        return "";
    }
}

static int parse_uint_bounded(const char *s, size_t max_val, size_t *out) {
    if (s == NULL || *s == '\0')
        return -1;

    size_t val = 0;
    for (const char *p = s; *p != '\0'; p++) {
        if (*p < '0' || *p > '9')
            return -1;
        size_t digit = (size_t)(*p - '0');
        if (val > (max_val - digit) / 10)
            return -1;
        val = val * 10 + digit;
    }

    *out = val;
    return 0;
}

ProtocolError parse_port(const char *s, uint16_t *out) {
    size_t val = 0;
    if (parse_uint_bounded(s, 65535, &val) < 0 || val == 0)
        return PROTO_ERR_INVALID_PORT;
    *out = (uint16_t)val;
    return PROTO_OK;
}

static ProtocolError parse_size(const char *s, size_t *out) {
    size_t val = 0;
    if (parse_uint_bounded(s, MAX_FILE_SIZE, &val) < 0)
        return PROTO_ERR_INVALID_SIZE;
    *out = val;
    return PROTO_OK;
}

static Command parse_command(const char *cmd) {
    if (strcmp(cmd, "REGISTER") == 0)
        return CMD_REGISTER;
    if (strcmp(cmd, "STORED") == 0)
        return CMD_STORED;
    if (strcmp(cmd, "PUT") == 0)
        return CMD_PUT;
    if (strcmp(cmd, "GET") == 0)
        return CMD_GET;
    if (strcmp(cmd, "LIST") == 0)
        return CMD_LIST;
    if (strcmp(cmd, "UPLOAD") == 0)
        return CMD_UPLOAD;
    if (strcmp(cmd, "DOWNLOAD") == 0)
        return CMD_DOWNLOAD;
    return CMD_UNKNOWN;
}

ProtocolError parse_request(char **fields, int count, Request *out) {
    if (fields == NULL || out == NULL || count <= 0)
        return PROTO_ERR_BAD_REQUEST;

    memset(out, 0, sizeof(*out));
    out->command = parse_command(fields[0]);

    switch (out->command) {
    case CMD_REGISTER:
        if (count != 2)
            return PROTO_ERR_BAD_REQUEST;
        return parse_port(fields[1], &out->storage_port);

    case CMD_STORED:
    case CMD_PUT:
    case CMD_UPLOAD: {
        if (count != 3)
            return PROTO_ERR_BAD_REQUEST;
        ProtocolError err = validate_remote_name(fields[1]);
        if (err != PROTO_OK)
            return err;
        err = parse_size(fields[2], &out->size);
        if (err != PROTO_OK)
            return err;
        strcpy(out->remote_name, fields[1]);
        return PROTO_OK;
    }

    case CMD_GET:
    case CMD_DOWNLOAD: {
        if (count != 2)
            return PROTO_ERR_BAD_REQUEST;
        ProtocolError err = validate_remote_name(fields[1]);
        if (err != PROTO_OK)
            return err;
        strcpy(out->remote_name, fields[1]);
        return PROTO_OK;
    }

    case CMD_LIST:
        if (count != 1)
            return PROTO_ERR_BAD_REQUEST;
        return PROTO_OK;

    case CMD_UNKNOWN:
    default:
        return PROTO_ERR_BAD_REQUEST;
    }
}
