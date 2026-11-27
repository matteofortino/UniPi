#ifndef HANDLERS_H
#define HANDLERS_H
#include <stdint.h>
#include "../coordinator.h"

int get_storage_slot_by_fd(int fd);
void clear_storage(int fd);
int handle_register(int fd, uint16_t port, uint32_t *out_id);
int find_file_slot(void);
int find_storage_slot(void);
int handle_stored(int fd, uint32_t sid, size_t size, char *remote_name);

#endif
