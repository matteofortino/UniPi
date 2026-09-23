#include "../internal.h"

using namespace heap;

void heap_init(void *start, size_t size)
{
	free_interna(start, size);
}
