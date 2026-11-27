#ifndef HANDLERS_H
#define HANDLERS_H
#include <stdint.h>
#include "../coordinator.h"
int find_storage_slot(Storage *storage);
int get_storage_slot_by_fd(int fd);
void clear_storage(int fd);
int handle_register(int fd, uint16_t port, uint32_t *out_id);
#endif
